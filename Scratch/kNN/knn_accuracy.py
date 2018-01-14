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

import random

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


    def calculate(self, measurement, train, k):
        count = 0
        for x in range(len(measurement)):
            nodeSet = measurement[x]
            for y in range(len(nodeSet)):
                #print("Measurement: {}".format(nodeSet[y]))
                neighbors = self.getNeighbors(train, nodeSet[y], k)
                #print("Get Neighbors: {}".format(neighbors))
                result = self.getResponse(neighbors)
                #print("Point: {} Voting: {}".format(x,result))

                if x == result:
                    count += 1
        #print(count)
        length = len(measurement[0])*len(measurement)
        #print(length)
        procent = float(count)/length*100
        #print("Accuracy: {}%".format(procent))
        print("{}   |{}".format(k,procent))




    def run(self):
        print("Started")
        input2 = raw_input("Split = ")
        splitProcent = int(input2)

        test = []
        train = []
        list = []
        list2 = []
        trainingSet = self.loadDataset()
        for x in range(len(trainingSet)):
            nodeSet = trainingSet[x]
            random.shuffle(nodeSet)
            split = len(nodeSet)*splitProcent/100

            for y in range(len(nodeSet)):
                if y < split:
                    list2 = []
                    list.append(nodeSet[y])
                else:
                    list = []
                    list2.append(nodeSet[y])


            train.append(list)
            test.append(list2)


        print("K   |Accuracy")

        for k in range(9):
            self.calculate(test, train, k+1)





if __name__ == "__main__":
    knn().run()
