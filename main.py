import math
import operator


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
        # print nodeSet
        for y in range(len(nodeSet[y])):
            # print nodeSet[y]
            dist = euclideanDistance(nodeSet[y], measurment, len(nodeSet))
            distances.append((node, dist))
            # print distances

    distances.sort(key=operator.itemgetter(1))
    #print distances

    neighbors = []
    for x in range(k):
        neighbors.append(distances[x][0])
    #print neighbors

    return neighbors


def numbers_to_strings(argument):
    switcher = {
        0: "a",
        1: "b",
        2: "c",
        3: "d",
    }
    return switcher.get(argument, "nothing")


def main():
    #Choose k so gateways that are too far will be dismissed
    k = 3

    #Test measurment
    measurment = [-65, -89, -73]

    #Test nodes
    testa = ([-60, -50, -40], [-61, -52, -40], [-62, -49, -41])
    testb = ([-98, -70, -60], [-97, -71, -61], [-93, -72, -61])
    testc = ([-80, -63, -79], [-82, -61, -77], [-81, -62, -80])
    testd = ([-64, -87, -70], [-63, -85, -73], [-63, -86, -71])
    trainingSet = [testa, testb, testc, testd]

    #Calculate the nearest neighbors
    neighbors = getNeighbors(trainingSet, measurment, k)
    result = getResponse(neighbors)
    print "Nearest neighbor is: " + result


main()
