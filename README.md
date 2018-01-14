# I-LpIoT
The buildup of the project is as follow:
- Backend: this contains everything around the backend,
- Final: this is the final and latest version of the mobil node,
- Ecompass: a full working ecompass module based on the LSM303AGR,
- IKS01A2_Driver: low power drivers for the X-NUCLEO-IKS01A2 motion MEMS and environmental sensor expansion board,
- Scratch: working pieces of the project that where made for testing purpose but coulkd still be usefull as examples, 
- Documentation: the documentation about every component used in the project.

## Project description
The project is a tracking/monitor system for the elderly in retirement homes. This system is capapble to track somebody indoors on multiple floors by using the kNN algorithm on the RSSI values from dash7 messages to different gateways. These dash7 messages contains sensordata. To know on which floor the person is we use a barometer. When the person is leaving the building the system will automatically switch to using a GPS for localization. The coordinates are send over LoRa instead over Dash7. Besides tracking a person the system will be able to check the temperature and direction that the person is going to via the ecompass. The nurse can monitor all the parameters on a backend and every mobile node has an alarm function where the user can activate an alarm for the nurse. 

![alt text](https://i.imgur.com/644kHPR.png "System setup")

## Hardware
The following hardware is used for this project:
- NUCLEO-L152RE (STM32 Nucleo-64 development board)
- 2 x B-L072Z-LRWAN1 LoRa®Discovery kit (LoRa & DASH7)
- X-NUCLEO-IKS01A2 is a motion MEMS and environmental sensor expansion board
- GLOBALSAT GPS Module - EM506
- A few resistors and transistors to toggle the modules (LoRa, DASH7 & Buzzer)
- Buzzer

![alt text](https://i.imgur.com/644kHPR.png "Hardware setup")

In this setup we used two small circuits to switch ON/OFF the pheriphals such as the GPS and LoRa modules with a transistor similar to this design.


![alt text](https://i.imgur.com/fONZKLY.jpg "Circuit")

A similar setup was used to toggle a buzzer. 


## Software
The mobile node is build in Eclipse. We suggest to use Mastering stm32 by Carmine Noviello to set up the IDE environment. The backend is build in PyCharm.
# Backend
The backend is a cloud based container (similar to a Raspberry PI 3). The received data from the mobile node will be parsed here and it handles the communication with Thingsboard.io.

## kNN
We use kNN for fingerprinting. We build a database of the environment where we track elderly people. The dataset is build by dividing the rooms in a grid and measure multiple times the RSSI values to the surroundig gateways. By sending sensor data to the backend we measure the RSSI value to four gateways. In this way we can compare the measurement with the dataset to locate the user.

![alt text](https://imgur.com/a/bEsyI.png "kNN")

# Frontend (mobile node)
The frontend is split into two different modes: indoor and outdoor. When the mobile node is indoors it will send sensordata over DASH7 to the backend. When the mobile node leaves the building the backend will send a message to the mobile node to switch over to LoRa communication and toggle the GPS to send coördinates periodically so that the backend can determine the outdoor position.

![alt text](https://i.imgur.com/vrYXFL3.png "Mobile node")

## Dash7
The DASH7-module was flashed as a slave. We send ALP commands containing the data to the gateways. 

## LoRa
The I-CUBE-LRWAN package contains an AT-SLAVE, this was uploaded to the LoRa Board. In this way the LoRa module can be used by sending AT-commands over UART. 

![alt text](https://i.imgur.com/hUEXywW.png "AT-slave")

The different AT-commands can be found in the application node.


## GPS
To get the position of the user the longtitude and latitude from the EM-506 GPS will be send to the Nucleo L152RE. The EM-506 GPS module uses uart to communicate with the Nucleo. In the datasheet we can find that the GPS uses NMEA commands. The NMEA commands has different formats.

In this project we use the GGL format which displays the following: $GPGLL, Latitude, N/S, Longtitude, E/W, Time, Data valid. The EM-506 module sends by default multiple NMEA command in different formats. To set the required GLL format whe use NMEA Input Commands. These can be found on page 12 in the datasheet. To generate the checksum we used following site: http://www.hhhh.org/wiml/proj/nmeaxor.html. For testing and setting the preferences we used an USB->TTL converter.

After setting the GPS in the right mode we get following data structure:
**$GPGLL,5117.1421,N,0000428.4897,E,230746.000,A,A*56**


## Ecompass
The sensor used for the ecompass is the LSM303AGR accelerometer and magnetometer found on the X-NUCLEO-IKS01A2 motion MEMS and environmental sensor expansion board. The algorithm is based on the Design tip DT0058 of ST (Computing tilt measurement and tilt-compensated e-compass). A timer is integrated in the ecompass to wake the NUCLEO-L152RE periodically for power reduction. Once the timer shoots an interrupt the NUCLEO-L152RE awakes, awekes the sensors, performs the calculation, put the sensors to sleep and at last goes to sleep itself untill the next interrupt is fired.

## IKS01A2
Like told in the buildup, there are written 2 drivers for the X-NUCLEO-IKS01A2 motion MEMS and environmental sensor expansion board. 1 for the LPS22HB (digital pressure and temperature sensor) and 1 for the LSM303AGR. The drivers are not so fancy low power implementations that are ready for use (no configuration needed from the user, also not possible). The algorithms are implemented in both drivers so the user can retreave the data he/she wishes.
