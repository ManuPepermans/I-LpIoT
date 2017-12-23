/**
 * LPS22HB_driver.h
 * Purpose: a driver for the LPS22HB sensor
 *
 * @author: Levi Mariën
 * @version 1.0 11/12/2017
 */

#include "stm32l1xx_hal.h"
#include "math.h"
#ifndef LPS22HB_DRIVER_H_
#define LPS22HB_DRIVER_H_

/*LPS22HB i2c address*/
#define LPS22HB_I2C_ADDRESS 0xBA //Transformed in 8-bit form

/*LPS22HB register map*/
#define LPS22HB_INTERRUPT_CFG	0x0B
#define LPS22HB_THS_P_L			0x0C
#define LPS22HB_THS_P_H			0x0E
#define LPS22HB_WHO_AM_I		0x0F
#define LPS22HB_CTRL_REG1		0x10
#define LPS22HB_CTRL_REG2		0x11
#define LPS22HB_CTRL_REG3		0x12
#define LPS22HB_FIFO_CTRL		0x14
#define	LPS22HB_REF_P_XL		0x15
#define LPS22HB_REF_P_L			0x16
#define LPS22HB_REF_P_H			0x17
#define LPS22HB_RPDS_L			0x18
#define LPS22HB_RPDS_H			0x19
#define LPS22HB_RES_CONF		0x1A
#define LPS22HB_INT_SOURCE		0x25
#define LPS22HB_FIFO_STATUS		0x26
#define LPS22HB_STATUS			0x27
#define LPS22HB_PRESS_OUT_XL	0x28
#define LPS22HB_PRES_OUT_L		0x29
#define LPS22HB_PRES_OUT_H		0x2A
#define LPS22HB_TEMP_OUT_L		0x2B
#define LPS22HB_TEMP_OUT_H		0x2C
#define LPS22HB_LPFP_RES		0x33

/*LPS22HB WHO_AM_I response*/
#define LPS22HB_WHO_AM_I_RSP 0xB1

/* Datatype definition */
typedef struct {
	uint8_t LSBpressure;
	uint8_t MIDpressure;
	uint8_t MSBpressure;
	uint8_t MSBtemp;
	uint8_t LSBtemp;
} LPS22HB_data;
uint32_t LPS22HB_pressure;
uint16_t LPS22HB_temperature;
uint8_t LPS22HB_settings;

HAL_I2C_StateTypeDef LPS22HB_status = HAL_OK;
I2C_HandleTypeDef *LPS22HB_hi2c;
UART_HandleTypeDef *LPS22HB_huart;

/*LPS22HB settings*/
#define LPS22HB_ONESHOT 0x11
#define LPS22HB_SWRESET 0x04
#define LPS22HB_BDU_CONTINIOUS 0x00
#define LPS22HB_BDU_NONCONTINUOUS 0x02
#define LPS22HB_ODR_POWERDOWN 0x00
#define LPS22HB_ODR_1HZ 0x80;

/*Function prototypes for the LPS22HB*/
void LPS22HB_setI2CInterface(I2C_HandleTypeDef *hi2c);
void LPS22HB_setUARTInterface(UART_HandleTypeDef *huart);
HAL_I2C_StateTypeDef LPS22HB_readRegister(uint8_t reg, uint8_t pData[]);
HAL_I2C_StateTypeDef LPS22HB_writeRegister(uint8_t reg, uint8_t pData[]);
void LPS22HB_configure();
void LPS22HB_reset();
void LPS22HB_powerdown();
void LPS22HB_wakeUp();
uint8_t LPS22HB_readPressure(LPS22HB_data *data);
uint8_t LPS22HB_readTemperature(LPS22HB_data *data);
//void LPS22HB_sendUART();

#endif /* LPS22HB_DRIVER_H_ */
