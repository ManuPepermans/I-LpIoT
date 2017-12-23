/*
 * LPS22HB_driver.cpp
 *
 *  Created on: 20 dec. 2017
 *      Author: levim
 */

#include "LPS22HB_driver.h"
#include "stm32l1xx_hal.h"

void LPS22HB_setI2CInterface(I2C_HandleTypeDef *hi2c) {

	LPS22HB_hi2c = hi2c;
}

void LPS22HB_setUARTInterface(UART_HandleTypeDef *huart) {

	LPS22HB_huart = huart;
}

HAL_I2C_StateTypeDef LPS22HB_readRegister(uint8_t reg, uint8_t pData[]) {

	return HAL_I2C_Mem_Read(LPS22HB_hi2c, LPS22HB_I2C_ADDRESS, reg,
	I2C_MEMADD_SIZE_8BIT, pData, sizeof(pData), HAL_MAX_DELAY);

}

HAL_I2C_StateTypeDef LPS22HB_writeRegister(uint8_t reg, uint8_t pData[]) {

	return HAL_I2C_Mem_Write(LPS22HB_hi2c, LPS22HB_I2C_ADDRESS, reg,
	I2C_MEMADD_SIZE_8BIT, pData, sizeof(pData), HAL_MAX_DELAY);

}

HAL_I2C_StateTypeDef LPS22HB_configure() {
	uint8_t hwID[1];
	LPS22HB_readRegister(LPS22HB_WHO_AM_I, hwID);
	if (hwID[0] == LPS22HB_WHO_AM_I_RSP) {
		LPS22HB_settings = LPS22HB_ONESHOT;
		if (LPS22HB_writeRegister(LPS22HB_CTRL_REG2, &LPS22HB_settings)
				== HAL_OK) {
			return HAL_OK;
		} else {
			return HAL_ERROR;
		}
	} else {
		LPS22HB_reset();
	}
}

void LPS22HB_reset() {

	LPS22HB_settings = LPS22HB_SWRESET;
	LPS22HB_writeRegister(LPS22HB_CTRL_REG2, LPS22HB_settings);
	HAL_Delay(10);
}

void LPS22HB_powerdown() {

}

uint8_t LPS22HB_readPressure(LPS22HB_data *data) {

}
uint8_t LPS22HB_readTemperature(LPS22HB_data *data) {

}

