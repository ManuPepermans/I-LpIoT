import csv
import paho.mqtt.client as mqtt
import json
import numpy as np
import pickle

metingen = np.zeros((10,4))   # Create an array of all zeros
resultaat = np.zeros((14,3))
FinalMatrix = np.zeros((0,4))


count1 = 1
count2 = 1
count3 = 1
count4 = 1
counter = 0
numberOfGateways = 4;
counterMax = 2

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
   print("Connected with result code "+str(rc))

   # Subscribing in on_connect() means that if we lose the connection and
   # reconnect then subscriptions will be renewed.
   client.subscribe("/localisation/#")
   client.subscribe("test/")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
   global count1, count2, count3, count4, counter, numberOfGateways, FinalMatrix
   print(msg.topic+" "+str(msg.payload))
   parsed_json = json.loads(str(msg.payload))
   if parsed_json['node'] == "43373134003e0041":
        print("Correcte node")
        print("counter", counter, "(count1 + count3) % (numberOfGateways * 11)", (count1 + count3) % (numberOfGateways * 11))
        #if counter < numberOfGateways and ((count1 + count2 + count3) % (NumberOfGateways * 11)) != 0:
        if counter < counterMax and ((count1 + count2 + count3 + count4) % (numberOfGateways * 11)) != 0:
            gateway = parsed_json['gateway']
            print("gateway: " + gateway)
            value = parsed_json['link_budget']
            print("RSS: " + str(value))

            if gateway == "c2c4ebd0-b95a-11e7-bebc-85e6dd10a2e8" and count1 < 11:
                metingen[count1-1,0] = value
                count1 += 1
                print("count1")
                print(count1)
            elif gateway == "b6b48ad0-b95a-11e7-bebc-85e6dd10a2e8" and count2 < 11:
                metingen[count2-1, 1] = value
                count2 += 1
                print("count2")
                print(count2)
            elif gateway == "f1f7e740-b8b0-11e7-bebc-85e6dd10a2e8" and count3 < 11:
                metingen[count3-1, 2] = value
                count3 += 1
                print("count3")
                print(count3)
            elif gateway == "43e01b20-b967-11e7-bebc-85e6dd10a2e8" and count4 < 11:
                metingen[count4-1, 3] = value
                count4 += 1
                print("count4")
                print(count4)
        #elif counter < numberOfGateways and ((count1 + count2 + count3) % (NumberOfGateways * 11)) == 0:
        elif counter < counterMax and ((count1 + count2 + count3 + count4) % (numberOfGateways * 11)) == 0:
            count1 = 1
            count2 = 1
            count3 = 1
            count4 = 1
            client.disconnect()
            print("metingen")
            print(metingen)
            test = np.mean(metingen, axis=0)
            print("test")
            print(test)
            #resultaat[counter-1, 0] = test[0]
            #resultaat[counter - 1, 1] = test[1]
            #resultaat[counter - 1, 2] = test[2]
            FinalMatrix = np.concatenate((FinalMatrix, metingen), axis=0)
            print("FinalMatrix")
            print(FinalMatrix)
            #FinalMatrix = np.concatenate((resultaat2, metingen), axis=0)
            input = raw_input("30 metingen verzameld. Hebt u zich al verplaatst voor de volgende meting?")
            print("Volgende meting begonnen!")
            client.connect("backend.idlab.uantwerpen.be", 1883, 60)
            counter += 1
        elif counter >= counterMax:
            print("metingen zijn afgelopen")
            client.disconnect()
            np.savetxt("foo.csv", FinalMatrix, delimiter=",")
            with open("trainingdata.csv", "wb") as f:
                writer = csv.writer(f)
                writer.writerows(FinalMatrix)




client = mqtt.Client(protocol=mqtt.MQTTv31)
client.on_connect = on_connect
client.on_message = on_message

client.connect("backend.idlab.uantwerpen.be", 1883, 60)
# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()
# Trainingsdata script
# Verzamel data bij een punt -> indien niet alle 4 gateways gevonden verwerpen (mss schaalbaar maken dat ge gemakellijk aantak gateways kunt aanpassen)
# Data moet verzameld worden zoals bv testa of test b en erna in die csv geplaatst worden



#training nodes
testa = ([-60, -50, -40], [-82, -61, -77], [-81, -62, -80], [-82, -61, -77], [-81, -62, -80])
testb = ([-80, -63, -79], [-82, -61, -77])
testc = ([-80, -63, -79], [-82, -61, -77], [-81, -62, -80])
testd = ([-64, -87, -70], [-63, -85, -73], [-63, -86, -71])
trainingSet = [testa, testb, testc, testd]

print trainingSet

with open("trainingdataTest.csv", "wb") as f:
    writer = csv.writer(f)
    writer.writerows(trainingSet)


