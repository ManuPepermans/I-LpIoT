# I-LpIoT
The buildup of the project is as follow:
- Backend: this contains everything around the backend,
- Final: this is the final and latest version of the mobil node,
- Ecompass: a full working ecompass module based on the LSM303AGR,
- IKS01A2_Driver: low power drivers for the X-NUCLEO-IKS01A2 motion MEMS and environmental sensor expansion board,
- Scratch: working pieces of the project that where made for testing purpose but coulkd still be usefull as examples, 
- Documentation: the documentation about every component used in the project.

## Explantation of the project
The project is a tracking/monitor system for the elderly in retirement homes. This system is capapble to track somebody indoors on multiple floors by using the kNN algorithm on the RSSI values from dash7 messages to different gateways. To know on which floor the person is we use a barometer. When the person is leaving the building the system will automatically switch to using a GPS for localization. The coordinates are send over LoRa instead over Dash7. Besides tracking a person the system will be able to check the temperature and direction that the person is going to via the ecompass. The nurse can monitor all the parameters on a backend and every mobile node has an alarm function where the user can activate an alarm for the nurce. 

## Hardware
The following hardware is used for this project:
- NUCLEO-L152RE (STM32 Nucleo-64 development board)
- B-L072Z-LRWAN1 LoRa®Discovery kit
- X-NUCLEO-IKS01A2 is a motion MEMS and environmental sensor expansion board
- GLOBALSAT GPS Module - EM506

![alt text](https://i.imgur.com/m5MR2cx.png "Hardware setup")

## Software
## Backend
The backend is based on a Raspberry PI 3. The received data from the vmobile node will be parsed here and it handles the communication with Thingsboard.io.

## Communication
Dash7 will be used to send data indoors from the mobil node to the backend. It will also be used for sending data from our backend to the node (i.e. warning signales). When the person leaves the retirement home communication will go via LoRa. periodically various kind of data will be send to the backend like the GPS coordinates, direction and the temperature.

## GPS
To get the positionlongtitude and latitude from the EM-506 GPS module with a Nucleo L152RE. The EM-506 GPS module uses uart to communicate with the Nucleo. In the datasheet we can find that the GPS uses NMEA commands. The NMEA commands has different formats.

In this project we use the GGL format which displays the following: $GPGLL, Latitude, N/S, Longtitude, E/W, Time, Data valid. The EM-506 module sends by default multiple NMEA command in different formats. To set the required GLL format whe use NMEA Input Commands. These can be found on page 12 in the datasheet. To generate the checksum we used following site: http://www.hhhh.org/wiml/proj/nmeaxor.html. For testing and setting the preferences we used an USB->TTL converter.

After setting the GPS in the right mode we get following data structure:
**$GPGLL,5117.1421,N,0000428.4897,E,230746.000,A,A*56**

## kNN
We use kNN for fingerprinting.

## ecompass
The sensor used for the ecompass is the LSM303AGR accelerometer and magnetometer found on the X-NUCLEO-IKS01A2 motion MEMS and environmental sensor expansion board. The algorithm is based on the Design tip DT0058 of ST (Computing tilt measurement and tilt-compensated e-compass). A timer is integrated in the ecompass to wake the NUCLEO-L152RE periodically for power reduction. Once the timer shoots an interrupt the NUCLEO-L152RE awakes, awekes the sensors, performs the calculation, put the sensors to sleep and at last goes to sleep itself untill the next interrupt is fired.

## IKS01A2
Like told in the buildup, there are written 2 drivers for the X-NUCLEO-IKS01A2 motion MEMS and environmental sensor expansion board. 1 for the LPS22HB (digital pressure and temperature sensor) and 1 for the LSM303AGR. The drivers are not so fancy low power implementations that are ready for use (no configuration needed from the user, also not possible). The algorithms are implemented in both drivers so the user can retreave the data he/she wishes.
