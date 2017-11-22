import math
import operator
import csv
import time
import paho.mqtt.client as mqtt
import json
import numpy as np
import datetime
count1 = 1
count2 = 1
count3 = 1
count4 = 1
firstRun = True
newValue = True
timestampBatch = 0
value1 = 0
value2 = 0
value3 = 0
value4 = 0
measurement = []



def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("/localisation/#")
        client.subscribe("test/")


def on_message(client, userdata, msg):
    #declaring some variables
    global timestamp1, timestamp2, timestamp3, timestamp4
        global value1, value2, value3, value4
            global timestampBatch
                global newValue
                    global firstRun
                        global measurement
                            #parse the JSON string, if the message contains the NODE ID capture it.
                            parsed_json = json.loads(str(msg.payload))
                            if parsed_json['node'] == "43373134003e0041":
                                #get the right values
gateway = parsed_json['gateway']
value = parsed_json['link_budget']
timestamp = parsed_json['timestamp']
date = datetime.datetime.strptime(timestamp, "%Y-%m-%d %H:%M:%S.%f")

# when new message was send from node, get a timestamp to check the nex incomonig messages
if newValue:
    print(timestampBatch)
    if (date.second < timestampBatch+1):
        if gateway == "c2c4ebd0-b95a-11e7-bebc-85e6dd10a2e8":
            value1 = value
                elif gateway == "b6b48ad0-b95a-11e7-bebc-85e6dd10a2e8":
                    value2 = value
            elif gateway == "f1f7e740-b8b0-11e7-bebc-85e6dd10a2e8":
                value3 = value
                elif gateway == "43e01b20-b967-11e7-bebc-85e6dd10a2e8":
                    value4 = value
            
            measurement = [value1, value2, value3, value4]
                
                print("Gateway: " + gateway + " RSS: " + str(value) + " Seconds : " + str(date.second))
            
            
            else:
                #fill the array when messages from gateways completed
                measurement = [value1, value2, value3, value4]
                print measurement
                #check for neighbours
                knn(measurement)
                newValue = False
                if not firstRun:
                    value1 = 0
                    value2 = 0
                    value3 = 0
                    value4 = 0
                
                if gateway == "c2c4ebd0-b95a-11e7-bebc-85e6dd10a2e8":
                    timestamp1 = datetime
                    value1 = value
                elif gateway == "b6b48ad0-b95a-11e7-bebc-85e6dd10a2e8":
                    timestamp2 = datetime
                    value2 = value
                elif gateway == "f1f7e740-b8b0-11e7-bebc-85e6dd10a2e8":
                    timestamp3 = datetime
                    value3 = value
                elif gateway == "43e01b20-b967-11e7-bebc-85e6dd10a2e8":
                    timestamp4 = datetime
                    value4 = value
                
                firstRun = False
                print("Gateway: " + gateway + " RSS: " + str(value) + " Seconds : " + str(date.second))


if not newValue:
    newValue = True
        timestampBatch = date.second


#Load the trainingdata and put it in the right format
def loadDataset(filename):
    with open(filename, 'rb') as csvfile:
        lines = csv.reader(csvfile)
        dataset = list(lines)
        trainingdata = []
        for x in range(len(dataset)):
            for y in range(len(dataset[x])):
                dataset[x][y] = dataset[x][y].replace("[","")
                dataset[x][y] = dataset[x][y].replace(" ", "")
                dataset[x][y] = dataset[x][y].replace("]", "")
                dataset[x][y] = dataset[x][y].replace("-", "")
                dataset[x][y] = dataset[x][y].split(',')
                dataset[x][y] = list(map(int, dataset[x][y]))
                trainingdata.append(dataset[x])
        return trainingdata



def euclideanDistance(instance1, instance2, length):
    distance = 0
    for x in range(length):
        distance += pow((instance1[x] - instance2[x]), 2)
    return math.sqrt(distance)


def getResponse(neighbors):
    classVotes = {}
    for x in range(len(neighbors)):
        response = neighbors[x]
        if response in classVotes:
            classVotes[response] += 1
        else:
            classVotes[response] = 1
    sortedVotes = sorted(classVotes.iteritems(), key=operator.itemgetter(1), reverse=True)
    return sortedVotes[0][0]


def getNeighbors(trainingSet, measurment, k):
    distances = []
    y = 0
    
    for x in range(len(trainingSet)):
        nodeSet = trainingSet[x]
        node = numbers_to_strings(x)
        test = len(nodeSet)
        for y in range(len(nodeSet)):
            lengte = len(nodeSet[y])
            dist = euclideanDistance(nodeSet[y], measurment, len(nodeSet[y]))
            distances.append((node, dist))
    # print distances

distances.sort(key=operator.itemgetter(1))
#print distances

neighbors = []
    for x in range(k):
        neighbors.append(distances[x][0])
print neighbors
    
    return neighbors


def numbers_to_strings(argument):
    switcher = {
        0: "a",
        1: "b",
        2: "c",
        3: "d",
        4: "e",
        5: "f"
    }
    return switcher.get(argument, "nothing")
def knn(measurement):
    k = 3
    trainingSet = loadDataset("trainingdata.csv")
    neighbors = getNeighbors(trainingSet, measurement, k)
    result = getResponse(neighbors)
    print "Nearest neighbor is: " + result

#Choose k so gateways that are too far will be dismissed
#global trainingSet



#Measurement: hier moet een meting gedaan worden en als er geen 4 waarden terug komen een 0 plaatsen . Zodra er een een meting is KNN functie aangroepen worden
#while(1):

#measurement = [80, 60, 80]
#knn(measurement, trainingSet)
#time.sleep(5)

client = mqtt.Client(protocol=mqtt.MQTTv31)
client.on_connect = on_connect
client.on_message = on_message

client.connect("backend.idlab.uantwerpen.be", 1883, 60)
# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.

client.loop_forever()
