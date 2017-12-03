import math
import operator
import csv
import paho.mqtt.client as mqtt
import json
import datetime

firstRun = True
newBatch = True
timestampBatch = 0
value1 = 0
value2 = 0
value3 = 0
value4 = 0
measurement = []
sample = 0
xPixel = 0
yPixel = 0


def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("/localisation/#")


def on_message(client, userdata, msg):
    # declaring some variables
    global value1, value2, value3, value4
    global timestampBatch
    global newBatch
    global firstRun
    global measurement

    global sample

    # parse the JSON string, if the message contains the NODE ID capture it.
    parsed_json = json.loads(str(msg.payload))
    if parsed_json['node'] == "43373134003e0041":
        # get the right values
        gateway = parsed_json['gateway']
        value = parsed_json['link_budget']
        timestamp = parsed_json['timestamp']
        date = datetime.datetime.strptime(timestamp, "%Y-%m-%d %H:%M:%S.%f")

        # when new message was send from node, check if the incoming messages are from the same batch as the timestampBatch
        if not newBatch:
            #print(timestampBatch)


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

                #print("Gateway: " + gateway + " RSS: " + str(value) + " Seconds : " + str(date.second))


            else:
                # fill the array when messages from gateways completed
                measurement = [value1, value2, value3, value4]
                print measurement
                # check for neighbours
                knn(measurement)
                newBatch = True
                if not firstRun:
                    value1 = 0
                    value2 = 0
                    value3 = 0
                    value4 = 0

                if gateway == "c2c4ebd0-b95a-11e7-bebc-85e6dd10a2e8":
                    value1 = value
                elif gateway == "b6b48ad0-b95a-11e7-bebc-85e6dd10a2e8":
                    value2 = value
                elif gateway == "f1f7e740-b8b0-11e7-bebc-85e6dd10a2e8":
                    value3 = value
                elif gateway == "43e01b20-b967-11e7-bebc-85e6dd10a2e8":
                    value4 = value

                firstRun = False
                #print("Gateway: " + gateway + " RSS: " + str(value) + " Seconds : " + str(date.second))

        if newBatch:
            newBatch = False
            timestampBatch = date.second
        if(date.second >= 58):
            sample = date.second
        if(date.second < sample):
            timestampBatch = 0
            sample = 0


# Load the trainingdata and put it in the right format
def loadDataset(filename):
    with open(filename, 'rb') as csvfile:
        lines = csv.reader(csvfile)
        dataset = list(lines)
        trainingdata = []
        for x in range(len(dataset)):
            for y in range(len(dataset[x])):
                dataset[x][y] = dataset[x][y].replace("[", "")
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
        for y in range(len(nodeSet)):
            dist = euclideanDistance(nodeSet[y], measurment, len(nodeSet[y]))
            distances.append((x, dist))

    distances.sort(key=operator.itemgetter(1))

    neighbors = []
    for x in range(k):
        neighbors.append(distances[x][0])
    #print neighbors
    return neighbors

def getPixels(results):
    global xPixel, yPixel
    if results == 0:
        xPixel = 1
        yPixel = 1
    if results == 1:
        xPixel = 1
        yPixel = 3
    if results == 2:
        xPixel = 1
        yPixel = 1
    if results == 3:
        xPixel = 1
        yPixel = 3
    if results == 4:
        xPixel = 1
        yPixel = 1
    if results == 5:
        xPixel = 1
        yPixel = 3
    if results == 6:
        xPixel = 1
        yPixel = 1
    if results == 7:
        xPixel = 1
        yPixel = 3
    if results == 8:
        xPixel = 1
        yPixel = 1
    if results == 9:
        xPixel = 1
        yPixel = 3
    if results == 10:
        xPixel = 1
        yPixel = 1
    if results == 11:
        xPixel = 1
        yPixel = 3
    if results == 12:
        xPixel = 1
        yPixel = 1
    if results == 13:
        xPixel = 1
        yPixel = 3
    if results == 14:
        xPixel = 1
        yPixel = 1
    if results == 15:
        xPixel = 1
        yPixel = 3
    if results == 16:
        xPixel = 1
        yPixel = 1
    if results == 17:
        xPixel = 1
        yPixel = 3
    if results == 18:
        xPixel = 1
        yPixel = 1
    if results == 19:
        xPixel = 1
        yPixel = 3
    if results == 20:
        xPixel = 1
        yPixel = 1
    if results == 21:
        xPixel = 1
        yPixel = 1
    if results == 22:
        xPixel = 1
        yPixel = 1
    if results == 23:
        xPixel = 1
        yPixel = 1
    if results == 24:
        xPixel = 1
        yPixel = 1
    if results == 25:
        xPixel = 1
        yPixel = 1
    if results == 26:
        xPixel = 1
        yPixel = 1

    return xPixel, yPixel

def knn(measurement):
    k = 3
    trainingSet = loadDataset("trainingdata.csv")
    neighbors = getNeighbors(trainingSet, measurement, k)
    result = getResponse(neighbors)
    print(getPixels(result))




client = mqtt.Client(protocol=mqtt.MQTTv31)
client.on_connect = on_connect
client.on_message = on_message

client.connect("backend.idlab.uantwerpen.be", 1883, 60)
# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.

client.loop_forever()
