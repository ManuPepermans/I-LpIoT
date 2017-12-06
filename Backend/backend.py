#!/usr/bin/env python
# coding=utf-8

from __future__ import print_function
import argparse
import socket
import subprocess

from datetime import datetime
import jsonpickle
import serial
import time

import paho.mqtt.client as mqtt
import signal

import struct

from d7a.alp.command import Command
from d7a.alp.operations.responses import ReturnFileData
from d7a.system_files.system_file_ids import SystemFileIds
from d7a.system_files.system_files import SystemFiles
from modem.modem import Modem

import time
from pprint import pprint

from tb_api_client import swagger_client
from tb_api_client.swagger_client import ApiClient, Configuration
from tb_api_client.swagger_client.rest import ApiException


class Backend:
    def __init__(self):
        # use cmd settings in script
        argparser = argparse.ArgumentParser()
        argparser.add_argument("-v", "--verbose", help="verbose", default=False, action="store_true")
        argparser.add_argument("-b", "--broker", help="mqtt broker hostname", default="localhost")
        argparser.add_argument("-u", "--url", help="URL of the thingsboard server", default="http://localhost:8080")
        argparser.add_argument("-t", "--token", help="token to access the thingsboard API", required=True)
        argparser.add_argument("-U", "--user", help="username for MQTT broker", required=True)
        argparser.add_argument("-P", "--password", help="password for MQTT broker", required=True)
        argparser.add_argument("-n", "--node", help="node name", default="4337313400210032")

        self.mq = None
        self.connected_to_mqtt = False

        self.config = argparser.parse_args()
        self.config.D7 = "/tb/#"
        self.connect_to_mqtt()

        api_client_config = Configuration()
        api_client_config.host = self.config.url
        api_client_config.api_key['X-Authorization'] = self.config.token
        api_client_config.api_key_prefix['X-Authorization'] = 'Bearer'
        api_client = ApiClient(api_client_config)

        self.device_controller_api = swagger_client.DeviceControllerApi(api_client=api_client)
        self.device_api_controller_api = swagger_client.DeviceApiControllerApi(api_client=api_client)

    def connect_to_mqtt(self):
        self.connected_to_mqtt = False

        # client_id=””, clean_session=True, userdata=None, protocol=MQTTv311, transport=”tcp”)
        self.mq = mqtt.Client("", True, None, mqtt.MQTTv31)

        self.mq.on_connect = self.on_mqtt_connect
        self.mq.on_message = self.on_mqtt_message

        # select broker
        self.mq.connect(self.config.broker, 1883, 60)
        self.mq.loop_start()

        while not self.connected_to_mqtt: pass  # busy wait until connected
        print("Connected to MQTT broker on {}".format(
            self.config.broker,
        ))

    def on_mqtt_connect(self, client, config, flags, rc):
        print("Connect to topic: {}".format(self.config.D7))

        # subscribe to topic
        self.mq.subscribe(self.config.D7)

        # login with username and password
        self.mq.username_pw_set(self.config.user, self.config.password)
        self.connected_to_mqtt = True

    def on_mqtt_message(self, client, config, msg):

        # msg contains already parsed command in ALP in JSON
        # print("ALP Command received from {}".format(msg.topic))
        try:
            obj = jsonpickle.json.loads(msg.payload)
        except:
            # print("Payload not valid JSON, skipping")
            return

        gateway = obj["deviceId"]
        cmd = jsonpickle.decode(jsonpickle.json.dumps(obj["alp"]))
        node_id = gateway  # overwritten below with remote node ID when received over D7 interface

        # get remote node id (when this is received over D7 interface)
        if cmd.interface_status != None and cmd.interface_status.operand.interface_id == 0xd7:
            # Convert from decimal to hexadecimal
            node_id = '{:x}'.format(cmd.interface_status.operand.interface_status.addressee.id)

        if node_id == self.config.node:
            print("***Right node: {}".format(node_id))

            for action in cmd.actions:
                if type(action.operation) is ReturnFileData and action.operand.offset.id == 1:

                    length = len(action.operand.data)
                    fmt = "c" * length
                    value_type = struct.unpack(fmt, bytearray(action.operand.data))[
                        0]  # parse binary payload (adapt to your needs)
                    print("Incoming data is from kind", format(value_type))

                    if value_type == "M":  # Magnetometer information
                        magn_value1 = struct.unpack(fmt, bytearray(action.operand.data))[1]
                        magn_value2 = struct.unpack(fmt, bytearray(action.operand.data))[2]
                        magn_value = int('{0:08b}'.format(ord(magn_value1)) + '{0:08b}'.format(ord(magn_value2)), 2)
                        # {} places a variable into a string, 0 takes the variable at argument position 0,
                        # : adds formatting options for this variable (otherwise it would represent decimal 6),
                        # 08 formats the number to eight digits zero-padded on the left,
                        # b converts the number to its binary representation
                        dir = self.magnetometer_to_direction(magn_value)
                        print("Direction is:", format(dir))
                        json_str = {"direction": dir}
                    elif value_type == "F": #RSSI value for fingerprinting
                        json_str = {"XPos": x}

            # save the parsed sensor data as an attribute to the device, using the TB API
            try:
                # first get the deviceId mapped to the device name
                response = self.device_controller_api.get_tenant_device_using_get(device_name=str(node_id))
                device_id = response.id.id
                # print(device_id)

                # next, get the access token of the device
                response = self.device_controller_api.get_device_credentials_by_device_id_using_get(device_id=device_id)
                device_access_token = response.credentials_id
                # print(device_access_token)

                # finally, store the sensor attribute on the node in TB
                response = self.device_api_controller_api.post_telemetry_using_post(
                    device_token=device_access_token,
                    json=json_str
                )

                print("Updated direction telemetry for node {}".format(node_id))
            except ApiException as e:
                print("Exception when calling API: %s\n" % e)
        else:
            # print("Wrong node: {}".format(node_id))
            return

    def magnetometer_to_direction(self, magn_value):
        if 338 < magn_value or magn_value <= 23:
            direction = "N"
        elif 23 < magn_value or magn_value <= 68:
            direction = "NE"
        elif 68 < magn_value or magn_value <= 113:
            direction = "E"
        elif 113 < magn_value or magn_value <= 158:
            direction = "SE"
        elif 158 < magn_value or magn_value <= 203:
            direction = "S"
        elif 203 < magn_value or magn_value <= 248:
            direction = "SW"
        elif 248 < magn_value or magn_value <= 293:
            direction = "W"
        elif 293 < magn_value or magn_value <= 338:
            direction = "NW"
        else:
            direction = "NULL"

        return direction

    def __del__(self):
        try:
            self.mq.loop_stop()
            self.mq.disconnect()
        except:
            pass

    def run(self):
        print("Started")
        keep_running = True
        while keep_running:
            try:
                signal.pause()
            except KeyboardInterrupt:
                print("received KeyboardInterrupt... stopping processing")
                keep_running = False


if __name__ == "__main__":
    Backend().run()
