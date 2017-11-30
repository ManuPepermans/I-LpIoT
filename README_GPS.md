# I-LpIoT
This project is made to get the longtitude and latitude from the EM-506 GPS module with a Nucleo F401RE. The EM-506 GPS module uses uart to communicate with the Nucleo. In the datasheet we can find that the GPS uses NMEA commands. The NMEA commands has different formats.

In this project we use the GGL format which displays the following: $GPGLL, Latitude, N/S, Longtitude, E/W, Time, Data valid. The EM-506 module sends by default multiple NMEA command in different formats. To set the required GLL format whe use NMEA Input Commands. These can be found on page 12 in the datasheet. To generate the checksum we used following site: http://www.hhhh.org/wiml/proj/nmeaxor.html. For testing and setting the preferences we used an USB->TTL converter.

The problem with HAL libraries is that they need a fixed length for a uart receive. So in this project DMA is used with circular buffer. The code is used from https://github.com/akospasztor/stm32-dma-uart/ and sends the data over uart. It uses a buffer, when the buffer is full it will send the buffer. In that way we get multiple messages in one buffer. So we still have to write a function on the board to put them together, or in Python on the backend. 

two example strings:
428.4992,E,230700.000,A,A*59 **$GPGLL,5117.1421,N,0000428.4897,E**
**,230746.000,A,A*56** $GPGLL,5117.1473,N,00428.4892,E,230748.000,A

resulting in: **$GPGLL,5117.1421,N,0000428.4897,E,230746.000,A,A*56**
