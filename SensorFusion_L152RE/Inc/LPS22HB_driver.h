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
#define LPS22HB_I2C_ADDRESS 0x5D

/*LPS22HB register map*/
#define INTERRUPT_CFG	0x0B
#define THS_P_L			0x0C
#define THS_P_H			0x0E
#define WHO_AM_I		0x10
#define CTRL_REG1		0x11
#define CTRL_REG2		0x12
#define CTRL_REG3		0x13
#define FIFO_CTRL		0x14
#define	REF_P_XL		0x15
#define REF_P_L			0x16
#define REF_P_H			0x17
#define RPDS_L			0x18
#define RPDS_H			0x19
#define RES_CONF		0x1A
#define INT_SOURCE		0x25
#define FIFO_STATUS		0x26
#define STATUS			0x27
#define PRESS_OUT_XL	0x28
#define PRES_OUT_L		0x29
#define PRES_OUT_H		0x2A
#define TEMP_OUT_L		0x2B
#define TEMP_OUT_H		0x2C
#define LPFP_RES		0x33

/*LPS22HB WHO_AM_I response*/
#define LPS22HB_WHO_AM_I_RSP 0xB1

/*LPS22HB settings*/

/*Function prototypes for the LPS22HB*/
I2C_HandleTypeDef *hi2cLib;
void setInterface_LPS22HB(I2C_HandleTypeDef *hi2c);
void reset_LPS22HB();
void reboot_LPS22HB();
void powerDown_LPS22HB();
uint8_t configure_LPS22HB();
uint8_t readRegister_LPS22HB(uint8_t buf[],uint8_t reg,uint8_t length);
uint8_t writeRegister_LPS22HB(uint8_t pData[],uint8_t length);




#endif /* LPS22HB_DRIVER_H_ */
