
#ifndef __GLOBAL_H
#define __GLOBAL_H
#include "stm32l1xx_hal.h"

#include <stdio.h>
#define LENGTH_ARRAY(x)  (sizeof(x) / sizeof(uint8_t))
#define FRAME_LENGTH 7
#define SERIAL 7
#define BUFSIZE 200

//functions
static void sendGPS(void);
static void initLora(void);
static void initGPS(void);
void DASH7Message(uint8_t[], int);
void loraError(void);

//typedefs
typedef int bool;
enum {
	false, true
};

//global variables
bool dangerZone;
bool loraJoined;
int loraCounter;

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif


#endif
