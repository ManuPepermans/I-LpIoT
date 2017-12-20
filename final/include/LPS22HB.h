//#include "main.h"
//#include <stdio.h>
//#include "stm32l1xx_it.h"
//#include "stm32l1xx_hal_conf.h"
//#include "stm32l1xx_hal.h"
//
//#ifndef __LS22HB_H
//#define __LS22HB_H
//
//
//
//
//
//#define LPS22HB_BARO_ADDRESS 0xBA // ADRESS from the barometer  "10111010"
//#define LPS22HB_AWHO_AM_I 0x0F
//#define LPS22HB_CTRL_REG2 0x11
//#define LPS22HB_PRESS_OUT_XL 0x28	// Pressure output value (LSB)
//#define LPS22HB_OUT_L 0x29			// Pressure output value (mid part)
//#define LPS22HB_OUT_H 0x2A			// Pressure output value (MSB)
//#define LPS22HB_TEMP_OUT_L 0x2B		// Temperature output value (LSB)
//#define LPS22HB_TEMP_OUT_H 0x2C		// Temperature output value (MSB)
////Set one shot mode for Barometer
//#define LPS22HB_ONE__SHOT_ENABLE 0x11 // 0001 0001
//
///* Datatype definition */
//typedef struct {
//uint8_t LSBpressure;
//uint8_t MIDpressure;
//uint8_t MSBpressure;
//} LPS22HB_TypeDef;
//
//
//uint8_t readBarometer(LPS22HB_TypeDef *data);
//
//
//
//#ifdef __cplusplus
// extern "C" {
//#endif
//void _Error_Handler(char *, int);
//
//#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
//#ifdef __cplusplus
//}
//#endif
//
//
//#endif
