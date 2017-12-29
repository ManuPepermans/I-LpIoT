/********************************************************
 * @author: Mariën Levi
 * @description: A low power driver for the LPS22HB sensor
 ********************************************************/

#include "stm32l1xx_hal.h"
#include "math.h"
#ifndef LPS22HB_DRIVER_H_
#define LPS22HB_DRIVER_H_

/*LPS22HB i2c address*/
//Transformed in 8-bit form
#define LPS22HB_I2C_ADDRESS 	0xBA

/*LPS22HB register map*/
#define LPS22HB_WHO_AM_I		0x0F
#define LPS22HB_CTRL_REG1		0x10
#define LPS22HB_CTRL_REG2		0x11
#define LPS22HB_CTRL_REG3		0x12
#define LPS22HB_STATUS			0x27
#define LPS22HB_PRESS_OUT_XL	0x28
#define LPS22HB_PRESS_OUT_L		0x29
#define LPS22HB_PRESS_OUT_H		0x2A
#define LPS22HB_TEMP_OUT_L		0x2B
#define LPS22HB_TEMP_OUT_H		0x2C

/*LPS22HB WHO_AM_I response*/
#define LPS22HB_WHO_AM_I_RSP 	0xB1

/* Datatype definition */
typedef union {
	uint8_t LSBpressure[1];
	uint8_t MIDpressure[1];
	uint8_t MSBpressure[1];
	uint32_t LPS22HB_pressureRaw;
	int LPS22HB_pressureHPa;
} LPS22HB_pressureData;
typedef union {
	uint8_t MSBtemp[1];
	uint8_t LSBtemp[1];
	uint16_t LPS22HB_temperatureRaw;
	int LPS22HB_temperature;
} LPS22HB_temperatureData;
typedef union {
	uint8_t temp_data_overrun;
	uint8_t press_data_overrun;
	uint8_t temp_data_available;
	uint8_t press_data_available;
} LPS22HB_statusRegister;

uint8_t LPS22HB_settings[1];
uint8_t LPS22HB_statusRegisterValue[1];

I2C_HandleTypeDef *LPS22HB_hi2c;

/*LPS22HB settings*/
#define LPS22HB_ODR_POWERDOWN 		0x00
#define LPS22HB_ODR_1HZ 			0x10
#define LPS22HB_ODR_10HZ 			0x20
#define LPS22HB_ODR_25HZ 			0x30
#define LPS22HB_ODR_50HZ 			0x40
#define LPS22HB_ODR_75HZ 			0x50
#define LPS22HB_BDU_CONTINIOUS 		0x00
#define LPS22HB_BDU_NONCONTINUOUS 	0x02
#define LPS22HB_BOOT 				0x80
#define LPS22HB_IF_ADD_INC 			0x02
#define LPS22HB_SWRESET 			0x04
#define LPS22HB_ONESHOT 			0x11
#define LPS22HB_LOW_NOISE			0x00
#define LPS22HB_LOW_CURRENT			0x01

/*Function prototypes for the LPS22HB*/
void LPS22HB_setI2CInterface(I2C_HandleTypeDef *hi2c);
HAL_I2C_StateTypeDef LPS22HB_readRegister(uint8_t reg, uint8_t pData[]);
HAL_I2C_StateTypeDef LPS22HB_writeRegister(uint8_t reg, uint8_t pData[]);
void LPS22HB_init();
void LPS22HB_reset();
void LPS22HB_powerdown();
void LPS22HB_setOneShotMode();
void LPS22HB_setLowCurrentMode();
void LPS22HB_setODR(uint8_t ODR);
uint8_t LPS22HB_getODR();
void LPS22HB_checkStatus();
void LPS22HB_getPressureAndTemperature(int* pressure, int* temperature);

#endif /* LPS22HB_DRIVER_H_ */
