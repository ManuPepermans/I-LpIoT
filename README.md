# I-LpIoT
The master will consist of one folder with the whole project and different smaller folders with pieces that where made sperately for testing purpose.
## Backend
Here we will pars the received data and comminucate with ThingsBoard.io

## Dash7
We use this  communication protocol to send data from the node to the gateways, which in turn will send it to idlab backend.
Dash7 will also be used for sending data from our backend to the node. This can be used for warning signals.

## GPS
This project is made to get the longtitude and latitude from the EM-506 GPS module with a Nucleo F401RE. The EM-506 GPS module uses uart to communicate with the Nucleo. In the datasheet we can find that the GPS uses NMEA commands. The NMEA commands has different formats.

In this project we use the GGL format which displays the following: $GPGLL, Latitude, N/S, Longtitude, E/W, Time, Data valid. The EM-506 module sends by default multiple NMEA command in different formats. To set the required GLL format whe use NMEA Input Commands. These can be found on page 12 in the datasheet. To generate the checksum we used following site: http://www.hhhh.org/wiml/proj/nmeaxor.html. For testing and setting the preferences we used an USB->TTL converter.

To receive the NMEA commands the program will capture and count all incoming characters and put into a buffer. If the CR LF is detected the command is completed in the buffer and we could use the counter value to cut the buffer or put the data in a new uint8_t array. 

resulting in: **$GPGLL,5117.1421,N,0000428.4897,E,230746.000,A,A*56**

## kNN
We use kNN for fingerprinting.

## Sensor Fusion
As example how to use the sesnors from the expansion board there is folder named SensorLSM303
