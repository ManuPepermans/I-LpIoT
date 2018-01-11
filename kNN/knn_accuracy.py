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
#Test node: "43373134003e0041"


class knn:

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

    def calculate(self, measurement, train, k):
        count = 0
        for x in range(len(measurement)):
            nodeSet = measurement[x]
            for y in range(len(nodeSet)):
                print("Step: {}".format(x))
                print("Measurement: {}".format(nodeSet[y]))
                neighbors = self.getNeighbors(train, nodeSet[y], k)
                print("Get Neighbors: {}".format(neighbors))
                result = self.getResponse(neighbors)
                print("Voting: {}".format(result))
                xPixel, yPixel = self.getPixels(result)
                print("Pixels: x = {} and y = {}".format(xPixel, yPixel))

                if x == result:
                    count += 1
                    print("correct")
        print(count)
        procent = float(count)/len(measurement)*100
        print(procent)




    def run(self):
        print("Started")
        input2 = raw_input("Split = ")
        split = int(input2)
        keep_running = True
        while keep_running:
            try:
                input = raw_input("K = ")
                k = int(input)

                test = []
                train = []
                list = []
                list2 = []
                trainingSet = self.loadDataset()
                print(train)
                for x in range(len(trainingSet)):
                    nodeSet = trainingSet[x]
                    for y in range(len(nodeSet)):

                        if y < split:
                            list2 = []
                            list.append(nodeSet[y])
                        else:
                            list = []
                            list2.append(nodeSet[y])
                    train.append(list)
                    test.append(list2)

                self.calculate(test, train, k)
            except KeyboardInterrupt:
                print("received KeyboardInterrupt... stopping processing")
                keep_running = False




if __name__ == "__main__":
    knn().run()
