# coding=utf-8
import math
import operator
import paho.mqtt.client as mqtt
import signal
import numpy as np

import json
import datetime
import argparse

import pickle

import ThingsBoard

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
#"43373134003e0041"

class knn:
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

        self.mqtt_config = None
        self.mqtt_connected = False

        self.config = argparser.parse_args()
        self.config.knn = "/localisation/#"
        self.connect_to_mqtt()

        ThingsBoard.start_api(self.config)

    def connect_to_mqtt(self):
        self.mqtt_connected = False

        # client_id=””, clean_session=True, userdata=None, protocol=MQTTv311, transport=”tcp”)
        self.mqtt_config = mqtt.Client("", True, None, mqtt.MQTTv31)

        self.mqtt_config.on_connect = self.on_mqtt_connect
        self.mqtt_config.on_message = self.on_mqtt_message

        # select broker
        self.mqtt_config.connect(self.config.broker, 1883, 60)
        self.mqtt_config.loop_start()

        while not self.mqtt_connected: pass  # busy wait until connected
        print("Connected to MQTT broker on {}".format(
            self.config.broker,
        ))

    def on_mqtt_connect(self, client, config, flags, rc):
        print("Connect to topic: {}".format(self.config.knn))

        # subscribe to topic
        self.mqtt_config.subscribe(self.config.knn)

        # login with username and password
        self.mqtt_config.username_pw_set(self.config.user, self.config.password)
        self.mqtt_connected = True

    def on_mqtt_message(self, client, config, msg):
        # declaring some variables
        global value1, value2, value3, value4
        global timestampBatch
        global newBatch
        global firstRun
        global measurement

        global sample

        try:
            obj = json.loads(msg.payload)
        except:
            print("Payload not valid JSON, skipping")
            return

        # parse the JSON string, if the message contains the NODE ID capture it.
        #parsed_json = json.loads(str(msg.payload))
        #print(parsed_json['node'])
        if obj['node'] == self.config.node:
            # get the right values
            gateway = obj['gateway']
            value = obj['link_budget']
            timestamp = obj['timestamp']
            date = datetime.datetime.strptime(timestamp, "%Y-%m-%d %H:%M:%S.%f")

            # when new message was send from node, check if the incoming messages are from the same batch as the timestampBatch
            if not newBatch:
                # print(timestampBatch)


                if (date.second < timestampBatch + 1):
                    if gateway == "c2c4ebd0-b95a-11e7-bebc-85e6dd10a2e8":
                        value1 = value
                    elif gateway == "b6b48ad0-b95a-11e7-bebc-85e6dd10a2e8":
                        value2 = value
                    elif gateway == "f1f7e740-b8b0-11e7-bebc-85e6dd10a2e8":
                        value3 = value
                    elif gateway == "43e01b20-b967-11e7-bebc-85e6dd10a2e8":
                        value4 = value

                    measurement = [value1, value2, value3, value4]

                    # print("Gateway: " + gateway + " RSS: " + str(value) + " Seconds : " + str(date.second))


                else:
                    # fill the array when messages from gateways completed
                    measurement = [value1, value2, value3, value4]
                    print measurement
                    # check for neighbours
                    self.calculate(measurement)
                    newBatch = True
                    if not firstRun:
                        value1 = np.nan
                        value2 = np.nan
                        value3 = np.nan
                        value4 = np.nan

                    if gateway == "c2c4ebd0-b95a-11e7-bebc-85e6dd10a2e8":
                        value1 = value
                    elif gateway == "b6b48ad0-b95a-11e7-bebc-85e6dd10a2e8":
                        value2 = value
                    elif gateway == "f1f7e740-b8b0-11e7-bebc-85e6dd10a2e8":
                        value3 = value
                    elif gateway == "43e01b20-b967-11e7-bebc-85e6dd10a2e8":
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
                #print("Wrong node")
            return


    # Load the trainingdata and put it in the right format
    def loadDataset(self):
        lists = pickle.load(open("a.pickle", "rb"))
        return lists

    def euclideanDistance(self, instance1, instance2, length):
        distance = 0
        for x in range(length):
            if not math.isnan(instance2[x]):
                distance += pow((instance1[x] - instance2[x]), 2)

        return math.sqrt(distance)

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

    def getNeighbors(self, trainingSet, measurment, k):
        distances = []
        y = 0

        for x in range(len(trainingSet)):
            nodeSet = trainingSet[x]
            for y in range(len(nodeSet)):
                dist = self.euclideanDistance(nodeSet[y], measurment, len(nodeSet[y]))
                distances.append((x, dist))

        distances.sort(key=operator.itemgetter(1))
        neighbors = []
        for x in range(k):
            neighbors.append(distances[x][0])
        # print neighbors
        return neighbors

    def getPixels(self, results):
        global xPixel, yPixel

        # Points on blueprint in pixels (blueprint = 1583 x 825 px)

        if results == 25:
            xPixel = 291
            yPixel = 203
        elif results == 24:
            xPixel = 409
            yPixel = 203
        elif results == 23:
            xPixel = 535
            yPixel = 203
        elif results == 22:
            xPixel = 673
            yPixel = 203
        elif results == 21:
            xPixel = 808
            yPixel = 203
        elif results == 20:
            xPixel = 932
            yPixel = 203
        elif results == 19:
            xPixel = 1055
            yPixel = 203
        elif results == 18:
            xPixel = 347
            yPixel = 328
        elif results == 17:
            xPixel = 471
            yPixel = 328
        elif results == 16:
            xPixel = 595
            yPixel = 328
        elif results == 15:
            xPixel = 746
            yPixel = 328
        elif results == 14:
            xPixel = 870
            yPixel = 328
        elif results == 13:
            xPixel = 994
            yPixel = 328
        elif results == 12:
            xPixel = 341
            yPixel = 409
        elif results == 11:
            xPixel = 449
            yPixel = 409
        elif results == 10:
            xPixel = 556
            yPixel = 409
        elif results == 9:
            xPixel = 664
            yPixel = 409
        elif results == 8:
            xPixel = 772
            yPixel = 409
        elif results == 7:
            xPixel = 879
            yPixel = 409
        elif results == 6:
            xPixel = 290
            yPixel = 536
        elif results == 5:
            xPixel = 397
            yPixel = 536
        elif results == 4:
            xPixel = 503
            yPixel = 536
        elif results == 3:
            xPixel = 611
            yPixel = 536
        elif results == 2:
            xPixel = 718
            yPixel = 536
        elif results == 1:
            xPixel = 829
            yPixel = 536
        elif results == 0:
            xPixel = 937
            yPixel = 536

        return xPixel, yPixel

    def calculate(self, measurement):
        count = measurement.count(np.nan)

        if count < 2:
            k = 5
            trainingSet = self.loadDataset()
            print("Measurement: {}".format(measurement))
            neighbors = self.getNeighbors(trainingSet, measurement, k)
            print("Get Neighbors: {}".format(neighbors))
            result = self.getResponse(neighbors)
            print("Voting: {}".format(result))
            xPixel, yPixel = self.getPixels(result)
            print("Pixels: x = {} and y = {}".format(xPixel,yPixel))
            #self.send_to_thingsboard(xPixel,yPixel)
            json_datax = {"x_group1": xPixel}
            ThingsBoard.send_json(self.config,json_datax)
            json_datay = {"y_group1": yPixel}
            ThingsBoard.send_json(self.config, json_datay)

    def __del__(self):
        try:
            self.mqtt_config.loop_stop()
            self.mqtt_config.disconnect()
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
    knn().run()
