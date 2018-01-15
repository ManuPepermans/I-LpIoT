#!/usr/bin/env python
# coding=utf-8

from __future__ import print_function
import argparse
import socket
import subprocess
import operator
import numpy as np
import json
import pickle
from datetime import datetime
import datetime
import jsonpickle
import serial
import time
from time import sleep
import paho.mqtt.client as mqtt
import signal
import struct
import math
import binascii
from d7a.alp.command import Command
from d7a.alp.operations.responses import ReturnFileData
from d7a.system_files.system_file_ids import SystemFileIds
from d7a.system_files.system_files import SystemFiles
from modem.modem import Modem
from pprint import pprint

import ThingsBoard
import ThingsBoardMQTT

firstRun = True
newBatch = True
timestampBatch = 0
value1 = np.nan
value2 = np.nan
value3 = np.nan
value4 = np.nan
measurement = []
sample = 0
xPixel = 0
yPixel = 0
referenceBaro = 1020
alert = 0
firstMeasurement = 1


class Backend:
    # The __init__ function will initiate the backend program with the correct MQTT config settings
    def __init__(self):
        # Use cmd settings in script
        argparser = argparse.ArgumentParser()
        argparser.add_argument("-v", "--verbose", help="verbose", default=False, action="store_true")
        argparser.add_argument("-b", "--broker", help="MQTT broker hostname", default="localhost")
        argparser.add_argument("-u", "--url", help="URL of the ThingsBoard server", default="http://localhost:8080")
        argparser.add_argument("-t", "--token", help="token to access the ThingsBoard API", required=True)
        argparser.add_argument("-tmqtt", "--tokenmqtt", help="token to access the ThingsBoard MQTT", required=True)
        argparser.add_argument("-U", "--user", help="username for MQTT broker", required=True)
        argparser.add_argument("-P", "--password", help="password for MQTT broker", required=True)
        argparser.add_argument("-n", "--node", help="node name", default="4337313400210032")

        self.mq = None
        self.connected_to_mqtt = False

        self.config = argparser.parse_args()

        # Topics
        self.config.D7 = "/tb/#"
        self.config.LoRa = "/loriot/#"
        self.config.knn = "/localisation/#"

        # Telemetry ThingsBoard
        self.config.telemetry = 'v1/gateway/telemetry'

        # Different gateways in the room
        self.config.gateway1 = "427ab180-f79e-11e7-8c87-85e6dd10a2e8"
        self.config.gateway2 = "b6b48ad0-b95a-11e7-bebc-85e6dd10a2e8"
        self.config.gateway3 = "f1f7e740-b8b0-11e7-bebc-85e6dd10a2e8"
        self.config.gateway4 = "43e01b20-b967-11e7-bebc-85e6dd10a2e8"

        self.connect_to_mqtt()

        # Start ThingsBoard API and MQTT
        ThingsBoard.start_api(self.config)
        ThingsBoardMQTT.start_mqtt("thingsboard.idlab.uantwerpen.be", self.config)

    # The connect_to_mqtt function starts the MQTT connection
    def connect_to_mqtt(self):
        self.connected_to_mqtt = False

        # client_id=””, clean_session=True, userdata=None, protocol=MQTTv311, transport=”tcp”)
        self.mq = mqtt.Client("", True, None, mqtt.MQTTv31)

        self.mq.on_connect = self.on_mqtt_connect
        self.mq.on_message = self.on_mqtt_message

        # Select broker
        self.mq.connect(self.config.broker, 1883, 60)
        self.mq.loop_start()

        while not self.connected_to_mqtt: pass  # busy wait until connected
        print("Connected to MQTT broker on {}".format(
            self.config.broker,
        ))

    # The on_mqtt_connect function subscribes to the topics: /tb, /localisation and /loriot.
    def on_mqtt_connect(self, client, config, flags, rc):
        print("Connect to topic: {}".format(self.config.D7))
        print("Connect to topic: {}".format(self.config.LoRa))
        print("Connect to topic: {}".format(self.config.knn))

        # Subscribe to topic
        self.mq.subscribe(self.config.D7)
        self.mq.subscribe(self.config.LoRa)
        self.mq.subscribe(self.config.knn)

        # Login with username and password
        self.mq.username_pw_set(self.config.user, self.config.password)
        self.connected_to_mqtt = True

    # The on_mqtt_message function runs when a new message is received on one of the MQTT topics. The function starts
    # the right parsing function for the different topics.
    def on_mqtt_message(self, client, config, msg):
        global alert

        # print("MQTT received from {}".format(msg.topic))

        # Check from which topic the message arrived
        if str(msg.topic) == "/loriot/":
            self.lorawan_topic(msg)
            self.alert()
        elif str(msg.topic) == "/tb":
            self.dash7_topic(msg)
        elif str(msg.topic) == "/localisation/DASH7":
            self.knn_topic(msg)
            self.alert()

    # The alert function sends the correct JSON string to ThingsBoard for the different alert states
    def alert(self):
        # Everything is OK state
        if alert == 0:
            json_str = {"alert": "Everything is OK"}
            ThingsBoard.send_json(self.config, json_str)

        # Alert: Partient outside state
        elif alert == 1:
            json_str = {"alert": "ALERT: Patient outside"}
            ThingsBoard.send_json(self.config, json_str)

        # Alert: LoRa not connected and patient outside state
        elif alert == 2:
            json_str = {"alert": "ALERT: LoRa not connected and patient outside"}
            ThingsBoard.send_json(self.config, json_str)

        # Alert: D7 message received and patient outside state
        elif alert == 3:
            json_str = {"alert": "ALERT: D7 message received and patient outside"}
            ThingsBoard.send_json(self.config, json_str)

        # Alert: LoRa received and patient outside state
        elif alert == 4:
            json_str = {"alert": "ALERT: LoRa received and patient outside"}
            ThingsBoard.send_json(self.config, json_str)

    # The knn_topic function will be run if message is received from the /localisation topic. The function checks if
    # it received the RSSI values from our node within the second.
    def knn_topic(self, msg):
        global value1, value2, value3, value4
        global timestampBatch
        global newBatch
        global firstRun
        global measurement
        global sample

        # Check if message is valid JSON
        try:
            obj = jsonpickle.json.loads(msg.payload)
        except:
            print("Payload not valid JSON, skipping")
            return

        # Parse the JSON string, if the message contains the NODE ID capture it.
        if obj['node'] == self.config.node:
            # get the right values
            gateway = obj['gateway']
            value = obj['link_budget']
            timestamp = obj['timestamp']
            date = datetime.datetime.strptime(timestamp, "%Y-%m-%d %H:%M:%S.%f")

            # When new message was send from node, check if the incoming messages are from the same batch as the
            # timestampBatch (within 1 second)
            if not newBatch:
                # print(timestampBatch)

                if (date.second < timestampBatch + 1):
                    if gateway == self.config.gateway1:
                        value1 = value
                    elif gateway == self.config.gateway2:
                        value2 = value
                    elif gateway == self.config.gateway3:
                        value3 = value
                    elif gateway == self.config.gateway4:
                        value4 = value

                    measurement = [value1, value2, value3, value4]
                    # print("Gateway: " + gateway + " RSS: " + str(value) + " Seconds : " + str(date.second))

                else:
                    # Fill the array when messages from gateways completed
                    measurement = [value1, value2, value3, value4]
                    # print(measurement)

                    # Calculate kNN
                    self.calculate(measurement)

                    newBatch = True
                    if not firstRun:
                        value1 = np.nan
                        value2 = np.nan
                        value3 = np.nan
                        value4 = np.nan

                    if gateway == self.config.gateway1:
                        value1 = value
                    elif gateway == self.config.gateway2:
                        value2 = value
                    elif gateway == self.config.gateway3:
                        value3 = value
                    elif gateway == self.config.gateway4:
                        value4 = value

                    firstRun = False
                    # print("Gateway: " + gateway + " RSS: " + str(value) + " Seconds : " + str(date.second))

            if newBatch:
                newBatch = False
                timestampBatch = date.second
            if (date.second >= 58):
                sample = date.second
            if (date.second < sample):
                timestampBatch = 0
                sample = 0
        else:
            # print("Wrong node")
            return

    # The dash7_topic function will be run if message is received from the /tb topic. The function parses the message
    # if it has the correct NODE ID
    def dash7_topic(self, msg):
        global json_str
        global referenceBaro
        global alert

        # Check if message is valid JSON and valid ALP
        try:
            obj = jsonpickle.json.loads(msg.payload)
            cmd = jsonpickle.decode(jsonpickle.json.dumps(obj["alp"]))
        except:
            #print("Payload not valid JSON, skipping")
            return

        # Overwritten below with remote node ID when received over D7 interface
        gateway = obj["deviceId"]
        node_id = gateway

        # Get remote node id (when this is received over D7 interface)
        if cmd.interface_status != None and cmd.interface_status.operand.interface_id == 0xd7:
            # Convert from decimal to hexadecimal
            node_id = '{:x}'.format(cmd.interface_status.operand.interface_status.addressee.id)

        # Check if message contains the right NODE ID
        if node_id == self.config.node:
            print("***Right node: {}".format(node_id))
            json_str = {}

            # Parse the D7 ALP message
            for action in cmd.actions:
                if type(action.operation) is ReturnFileData and action.operand.offset.id == 64:

                    # Measure length of ALP data and change to "c"har
                    length = len(action.operand.data)
                    fmt = "c" * length

                    # Parse the firtst binary payload to know of which kind the message is
                    value_type = struct.unpack(fmt, bytearray(action.operand.data))[0]
                    print("Incoming data is from kind", format(value_type))

                    # Magnetometer information
                    if value_type == "M":
                        magn_value1 = struct.unpack(fmt, bytearray(action.operand.data))[1]
                        magn_value2 = struct.unpack(fmt, bytearray(action.operand.data))[2]
                        magn_value = int('{0:08b}'.format(ord(magn_value1)) + '{0:08b}'.format(ord(magn_value2)), 2)
                        # {} places a variable into a string, 0 takes the variable at argument position 0,
                        # : adds formatting options for this variable (otherwise it would represent decimal 6),
                        # 08 formats the number to eight digits zero-padded on the left,
                        # b converts the number to its binary representation

                        print("Magnotometer value: ", format(magn_value))

                        # Change degrees to cardinal direction
                        dir = self.magnetometer_to_direction(magn_value)
                        print("Direction is: ", format(dir))

                        # Send to ThingsBoard using API
                        json_str = {"direction": dir}
                        ThingsBoard.send_json(self.config, json_str)

                    # Barometer information
                    elif value_type == "B":
                        baro_value1 = struct.unpack(fmt, bytearray(action.operand.data))[1]
                        baro_value2 = struct.unpack(fmt, bytearray(action.operand.data))[2]
                        baro_value3 = struct.unpack(fmt, bytearray(action.operand.data))[3]
                        temp_value1 = struct.unpack(fmt, bytearray(action.operand.data))[4]
                        temp_value2 = struct.unpack(fmt, bytearray(action.operand.data))[5]

                        baro_value = int(
                            '{0:08b}'.format(ord(baro_value1)) + '{0:08b}'.format(
                                ord(baro_value2)) + '{0:08b}'.format(
                                ord(baro_value3)), 2)

                        temperature = int(
                            '{0:08b}'.format(ord(temp_value1)) + '{0:08b}'.format(
                                ord(temp_value2)), 2)
                        temperature = float(temperature) / 100
                        print("Temperature is: ", format(temperature))

                        # Send to ThingsBoard using API
                        json_str = {"temp": temperature}
                        ThingsBoard.send_json(self.config, json_str)

                        # Calculate altitude form barometer information
                        altitude = self.barometer_calculate(float(baro_value), temperature)
                        print("Altitude is: ", format(altitude))
                        print("Reference Barometer is: ", format(referenceBaro))

                        # Calculate level from altitude
                        level = (altitude + 2) / 4

                        # Send to ThingsBoard using API
                        #json_str = {"level": altitude}
                        json_str = {"level": level}
                        ThingsBoard.send_json(self.config, json_str)

                    # D7 message received by node
                    elif value_type == "O":
                        # Send to ThingsBoard using API
                        json_str = {"alert": "ALERT: D7 message received and patient outside"}
                        ThingsBoard.send_json(self.config, json_str)
                        alert = 3

                    # LoRa error message, LoRaWAN not connected
                    elif value_type == "E":
                        # Send to ThingsBoard using API
                        json_str = {"alert": "ALERT: LoRa not connected and patient outside"}
                        ThingsBoard.send_json(self.config, json_str)
                        alert = 2

        else:
            # print("Wrong node: {}".format(node_id))
            return

    # The lorawan_topic function will be run if message is received from the /loriot topic. The function parses the
    # message if it has the correct EUI
    def lorawan_topic(self, msg):
        global alert

        # Check if message is valid JSON
        try:
            obj = jsonpickle.json.loads(msg.payload)
        except:
            # print("Payload not valid JSON, skipping")
            return

        # Parse the JSON string, if the message contains the correct EUI capture it.
        if obj["EUI"] == "BE7A000000001B94" and obj["cmd"] == "rx":
            print("***Right node: {}".format(obj["EUI"]))

            # Change alert status
            alert = 4
            self.alert()

            # Capture the data part of the message
            cmd = jsonpickle.decode(jsonpickle.json.dumps(obj["data"]))

            # Decode the data and split it in different values
            [GPGLL, latitude, la, longitude, lon, time, valid, A2] = cmd.decode('hex').split(',')

            # Check if GPS data is valid (A)
            if valid == "A":

                # Calculate latitude, change from minutes to degrees
                latitude = float(latitude)
                deg = math.floor(latitude / 100)
                min = latitude - (deg * 100)

                latitude = deg + min / 60

                # Calculate longitude, change from minutes to degrees
                longitude = float(longitude)
                degl = math.floor(longitude / 100)
                minl = longitude - (degl * 100)

                longitude = degl + minl / 60

                # Change latitude and longitude if GPS data is from Southern Hemisphere
                if la == 'S':
                    latitude *= -1
                if lon == 'W':
                    longitude *= -1

                print("GPS value: lat = {} and lon = {}".format(la, lon))

                # Publish MQTT to Thingsboard
                ThingsBoardMQTT.on_mqtt_publish(latitude, longitude)
            else:
                print("No valid GPS")

    # The loadDataset function will load the trainingdata and put it in the right format
    def loadDataset(self):
        lists = pickle.load(open("a.pickle", "rb"))
        return lists

    # The euclideanDistance function will calculate the euclidean distance between two instances and returns the
    # calculated distance
    def euclideanDistance(self, instance1, instance2, length):
        distance = 0
        for x in range(length):
            if not math.isnan(instance2[x]):
                distance += pow((instance1[x] - instance2[x]), 2)

        return math.sqrt(distance)

    # The getResponse function is the voting algorithm of the kNN function. It will count how many times the measurement
    # was on a specific place.
    def getResponse(self, neighbors):
        classVotes = {}
        for x in range(len(neighbors)):
            response = neighbors[x]
            if response in classVotes:
                classVotes[response] += 1
            else:
                classVotes[response] = 1
        sortedVotes = sorted(classVotes.iteritems(), key=operator.itemgetter(1), reverse=True)
        return sortedVotes[0][0]

    # The getNeighbors function compares the measurement with the training dataset, calculates distance and sorts the
    # array from smallest to biggest.
    def getNeighbors(self, trainingSet, measurment, k):
        distances = []
        y = 0

        for x in range(len(trainingSet)):
            nodeSet = trainingSet[x]
            for y in range(len(nodeSet)):
                # Calculate euclidean distance
                dist = self.euclideanDistance(nodeSet[y], measurment, len(nodeSet[y]))
                distances.append((x, dist))

        # Sort array from small to big distance
        distances.sort(key=operator.itemgetter(1))
        neighbors = []

        # Only keep k smallest distances
        for x in range(k):
            neighbors.append(distances[x][0])

        return neighbors

    # The getPixels function gives the pixels from a corresponding point
    def getPixels(self, results):
        global xPixel, yPixel
        global alert

        # Points on blueprint in pixels (blueprint = 1583 x 825 px)
        if results == 0:
            xPixel = 937
            yPixel = 536
        elif results == 1:
            xPixel = 829
            yPixel = 536
        elif results == 2:
            xPixel = 718
            yPixel = 536
        elif results == 3:
            xPixel = 611
            yPixel = 536
        elif results == 4:
            xPixel = 503
            yPixel = 536
        elif results == 5:
            xPixel = 397
            yPixel = 536
        elif results == 6:
            xPixel = 290
            yPixel = 536
        elif results == 7:
            xPixel = 341
            yPixel = 409
        elif results == 8:
            xPixel = 449
            yPixel = 409
        elif results == 9:
            xPixel = 556
            yPixel = 409
        elif results == 10:
            xPixel = 664
            yPixel = 409
        elif results == 11:
            xPixel = 772
            yPixel = 409
        elif results == 12:
            xPixel = 879
            yPixel = 409
        elif results == 13:
            xPixel = 994
            yPixel = 328
        elif results == 14:
            xPixel = 870
            yPixel = 328
        elif results == 15:
            xPixel = 746
            yPixel = 328
        elif results == 16:
            xPixel = 595
            yPixel = 328
        elif results == 17:
            xPixel = 471
            yPixel = 328
        elif results == 18:
            xPixel = 347
            yPixel = 328
        elif results == 19:
            xPixel = 291
            yPixel = 203
        elif results == 20:
            xPixel = 409
            yPixel = 203
        elif results == 21:
            xPixel = 535
            yPixel = 203
        elif results == 22:
            xPixel = 673
            yPixel = 203
        elif results == 23:
            xPixel = 808
            yPixel = 203
        elif results == 24:
            xPixel = 932
            yPixel = 203
        elif results == 25:
            xPixel = 1055
            yPixel = 203
        elif results == 26:
            xPixel = 1183
            yPixel = 412

            # Change alert status
            if alert == 0:
                print("*****Danger-zone*****")
                alert = 1

        # Activate alert
        if alert == 1:
            self.alert()
            self.sendRPC()

        return xPixel, yPixel

    # The calculate function calculates kNN
    def calculate(self, measurement):
        # Count how many NAN measurements there are
        count = measurement.count(np.nan)

        # Calculate if you have RSSI from at least 3 gateways
        if count < 2:
            # Use k smallest distances
            k = 3

            # Load the training set
            trainingSet = self.loadDataset()

            print("Measurement: {}".format(measurement))

            # Calculate neighbors
            neighbors = self.getNeighbors(trainingSet, measurement, k)
            print("Get Neighbors: {}".format(neighbors))

            # Voting algorithm
            result = self.getResponse(neighbors)
            print("Voting: {}".format(result))

            # Get pixels of voted point
            xPixel, yPixel = self.getPixels(result)
            print("Pixels: x = {} and y = {}".format(xPixel, yPixel))

            # Send to ThingsBoard
            json_datax = {"x_group1": xPixel}
            ThingsBoard.send_json(self.config, json_datax)
            json_datay = {"y_group1": yPixel}
            ThingsBoard.send_json(self.config, json_datay)

    # The barometer_calculate function will calculate the altitude form the raw barometer information
    def barometer_calculate(self, baro_value, temperature):
        global firstMeasurement
        global referenceBaro
        pressureHPA = baro_value / 4096

        if firstMeasurement == 1:
            referenceBaro = pressureHPA
            firstMeasurement = 0

        intermediate1 = pow((referenceBaro / pressureHPA), 0.1902)
        intermediate2 = intermediate1 - 1
        intermediate3 = intermediate2 * (temperature + 273.15)
        relativeHeight = intermediate3 / 0.0065

        return relativeHeight

    # The magnetometer_to_direction function will change the degrees data to cardinal direction
    def magnetometer_to_direction(self, magn_value):
        if 338 < magn_value and magn_value <= 23:
            direction = "N"
        elif 23 < magn_value and magn_value <= 68:
            direction = "NE"
        elif 68 < magn_value and magn_value <= 113:
            direction = "E"
        elif 113 < magn_value and magn_value <= 158:
            direction = "SE"
        elif 158 < magn_value and magn_value <= 203:
            direction = "S"
        elif 203 < magn_value and magn_value <= 248:
            direction = "SW"
        elif 248 < magn_value and magn_value <= 293:
            direction = "W"
        elif 293 < magn_value and magn_value <= 338:
            direction = "NW"
        else:
            direction = "NULL"

        return direction

    # The sendRPC function will send a remote procedure call from every gateway to the node.
    def sendRPC(self):
        for x in range(5):
            print("Alert {} to node".format(x))
            try:
                ThingsBoard.execute_rpc_command(self.config.gateway1,
                                                [0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31])
            except:
                print("Gateway {} not reached.".format(self.config.gateway1))
            sleep(0.5)
            try:
                ThingsBoard.execute_rpc_command(self.config.gateway2,
                                                [0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31])
            except:
                print("Gateway {} not reached.".format(self.config.gateway2))
            sleep(0.5)
            try:
                ThingsBoard.execute_rpc_command(self.config.gateway3,
                                                [0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31])
            except:
                print("Gateway {} not reached.".format(self.config.gateway3))
            sleep(0.5)
            try:
                ThingsBoard.execute_rpc_command(self.config.gateway4,
                                                [0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31])
            except:
                print("Gateway {} not reached.".format(self.config.gateway4))
            sleep(2)

    # The __del__ function will stop the MQTT loop
    def __del__(self):
        try:
            self.mq.loop_stop()
            self.mq.disconnect()
        except:
            pass

    # The run function will start the backend and also stop it when a keyboard interrupt is given
    def run(self):
        global alert
        print("Started")
        alert = 0
        keep_running = True
        while keep_running:
            try:
                signal.pause()
            except KeyboardInterrupt:
                print("received KeyboardInterrupt... stopping processing")
                keep_running = False


if __name__ == "__main__":
    Backend().run()
