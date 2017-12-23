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

import ThingsBoard
import ThingsBoardMQTT

temperature = 21


class Backend:
    def __init__(self):
        # use cmd settings in script
        argparser = argparse.ArgumentParser()
        argparser.add_argument("-v", "--verbose", help="verbose", default=False, action="store_true")
        argparser.add_argument("-b", "--broker", help="mqtt broker hostname", default="localhost")
        argparser.add_argument("-u", "--url", help="URL of the ThingsBoard server", default="http://localhost:8080")
        argparser.add_argument("-t", "--token", help="token to access the ThingsBoard API", required=True)
        argparser.add_argument("-tmqtt", "--tokenmqtt", help="token to access the ThingsBoard mqtt", required=True)
        argparser.add_argument("-U", "--user", help="username for MQTT broker", required=True)
        argparser.add_argument("-P", "--password", help="password for MQTT broker", required=True)
        argparser.add_argument("-n", "--node", help="node name", default="4337313400210032")

        self.mq = None
        self.connected_to_mqtt = False

        self.config = argparser.parse_args()
        self.config.D7 = "/tb/#"
        self.config.LoRa = "/loriot/#"
        self.config.telemetry = 'v1/gateway/telemetry'
        self.connect_to_mqtt()

        ThingsBoard.start_api(self.config)
        #ThingsBoardMQTT.start_mqtt("thingsboard.idlab.uantwerpen.be", "98qEWhtBm4FRzWGTvIER", 'v1/gateway/telemetry')
        ThingsBoardMQTT.start_mqtt(self.config.url, self.config.tokenmqtt, self.config.telemetry)

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
        print("Connect to topic: {}".format(self.config.LoRa))

        # subscribe to topic
        self.mq.subscribe(self.config.D7)
        self.mq.subscribe(self.config.LoRa)

        # login with username and password
        self.mq.username_pw_set(self.config.user, self.config.password)
        self.connected_to_mqtt = True

    def on_mqtt_message(self, client, config, msg):
        print("MQTT received from {}".format(msg.topic))

        if str(msg.topic) == "/loriot/":
            self.lorawan_topic(msg)
        elif str(msg.topic) == "/tb/":
            self.dash7_topic(msg)

    def lorawan_topic(self, msg):
        try:
            obj = jsonpickle.json.loads(msg.payload)
        except:
            # print("Payload not valid JSON, skipping")
            return

        node_id = obj["EUI"]
        if node_id == "BE7A000000001B94":
            print("***Right node: {}".format("BE7A000000001B94"))

            cmd = jsonpickle.decode(jsonpickle.json.dumps(obj["data"]))

            #5117.1421,N,0000428.4897,E,230746.000,A,A*56

        value_type = "G"
        print("Parse LORA")
        if value_type == "G":  # GPS information form loriot
            # gps_value1 = struct.unpack(fmt, bytearray(action.operand.data))[1]
            # gps_value2 = struct.unpack(fmt, bytearray(action.operand.data))[2]

            ThingsBoardMQTT.on_mqtt_publish(51.1776, 4.4149)

    def dash7_topic(self, msg):
        global json_str

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
            json_str = {}

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
                        print("Direction is: ", format(dir))
                        json_str = {"direction": dir}
                        ThingsBoard.send_json(self.config, json_str)
                    elif value_type == "B":  # Barometer information
                        baro_value1 = struct.unpack(fmt, bytearray(action.operand.data))[1]
                        baro_value2 = struct.unpack(fmt, bytearray(action.operand.data))[2]
                        baro_value3 = struct.unpack(fmt, bytearray(action.operand.data))[3]
                        baro_value = int(
                            '{0:08b}'.format(ord(baro_value1)) + '{0:08b}'.format(
                                ord(baro_value2)) + '{0:08b}'.format(
                                ord(baro_value3)), 2)

                        level = self.barometer_calculate(baro_value)
                        print("Level is: ", format(level))
                        json_str = {"level": level}
                        ThingsBoard.send_json(self.config, json_str)
                    elif value_type == "T":  # Temperature information
                        global temperature
                        temp_value1 = struct.unpack(fmt, bytearray(action.operand.data))[1]
                        temp_value2 = struct.unpack(fmt, bytearray(action.operand.data))[2]
                        temp_value = int('{0:08b}'.format(ord(temp_value1)) + '{0:08b}'.format(ord(temp_value2)), 2)

                        print("Temperature is: ", format(temp_value))
                        json_str = {"Temperature": temp_value}
                        ThingsBoard.send_json(self.config, json_str)

        else:
            # print("Wrong node: {}".format(node_id))
            return

    def barometer_calculate(self, baro_value):
        global temperature

        pressurehpa = baro_value / 4096
        top = pow((1001.1 / pressurehpa), 0.1902)
        top1 = top - 1
        top2 = top1 * (temperature + 273.15)
        top3 = top2 / 0.0065

        return top3

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
