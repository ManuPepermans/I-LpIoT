# I-LpIoT
The master will consist of one folder for the mobile node with the whole project and one folder for the backend. The other folders are working pieces of the project that where made sperately for testing purpose but could still be usefull as examples. The source code for the mobile node will be put in the 'final' folder.

## Explantation of the project
The project is a tracking/monitor system for the elderly in retirement homes. This system is capapble to track somebody indoors on multiple floors by using the kNN algorithm on the RSSI values from dash7 messages to different gateways. To know on which floor the person is we use a barometer. When the person is leaving the building the system will automatically switch to using a GPS for localization. The coordinates are send over LoRa instead over Dash7. Besides tracking a person the system will be able to check the temperature and direction that the person is going to. The nurse can monitor all the parameters on a backend and every mobile node has an alarm function where the user can activate an alarm for the nurce. 

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

## Dash7
We use Dash7 to send local data from the mobile node to the backend. Dash7 will also be used for sending data from our backend to the node (i.e. warning signals).

## GPS
To get the positionlongtitude and latitude from the EM-506 GPS module with a Nucleo L152RE. The EM-506 GPS module uses uart to communicate with the Nucleo. In the datasheet we can find that the GPS uses NMEA commands. The NMEA commands has different formats.

In this project we use the GGL format which displays the following: $GPGLL, Latitude, N/S, Longtitude, E/W, Time, Data valid. The EM-506 module sends by default multiple NMEA command in different formats. To set the required GLL format whe use NMEA Input Commands. These can be found on page 12 in the datasheet. To generate the checksum we used following site: http://www.hhhh.org/wiml/proj/nmeaxor.html. For testing and setting the preferences we used an USB->TTL converter.

After setting the GPS in the right mode we get following data structure:
**$GPGLL,5117.1421,N,0000428.4897,E,230746.000,A,A*56**

## kNN
We use kNN for fingerprinting.

## Sensor Fusion
As example how to use the sesnors from the expansion board there is folder named SensorLSM303
