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

HAL_I2C_StateTypeDef LPS22HB_readRegister(uint8_t reg, uint8_t pData[]) {

	return HAL_I2C_Mem_Read(LPS22HB_hi2c, LPS22HB_I2C_ADDRESS, reg,
	I2C_MEMADD_SIZE_8BIT, pData, sizeof(pData), HAL_MAX_DELAY);

}

HAL_I2C_StateTypeDef LPS22HB_writeRegister(uint8_t reg, uint8_t pData[]) {

	return HAL_I2C_Mem_Write(LPS22HB_hi2c, LPS22HB_I2C_ADDRESS, reg,
	I2C_MEMADD_SIZE_8BIT, pData, sizeof(pData), HAL_MAX_DELAY);

}

void LPS22HB_init() {
	LPS22HB_settings[0] = LPS22HB_ODR_POWERDOWN;
	LPS22HB_writeRegister(LPS22HB_CTRL_REG1, LPS22HB_settings);
	LPS22HB_settings[0] = LPS22HB_ONESHOT;
	LPS22HB_writeRegister(LPS22HB_CTRL_REG2, LPS22HB_settings);
}

void LPS22HB_setODR(uint8_t ODR) {
	uint8_t controlRegisterValue[1];
	LPS22HB_readRegister(LPS22HB_CTRL_REG1, controlRegisterValue);
	uint8_t mask = 0b01110000;
	controlRegisterValue[0] &= ~mask;
	switch (ODR) {
	case (uint8_t) LPS22HB_ODR_POWERDOWN:
		LPS22HB_settings[0] = controlRegisterValue[0] | LPS22HB_ODR_POWERDOWN;
		break;
	case (uint8_t) LPS22HB_ODR_1HZ:
		LPS22HB_settings[0] = controlRegisterValue[0] | LPS22HB_ODR_1HZ;
		break;
	case (uint8_t) LPS22HB_ODR_10HZ:
		LPS22HB_settings[0] = controlRegisterValue[0] | LPS22HB_ODR_10HZ;
		break;
	case (uint8_t) LPS22HB_ODR_25HZ:
		LPS22HB_settings[0] = controlRegisterValue[0] | LPS22HB_ODR_25HZ;
		break;
	case (uint8_t) LPS22HB_ODR_50HZ:
		LPS22HB_settings[0] = controlRegisterValue[0] | LPS22HB_ODR_50HZ;
		break;
	case (uint8_t) LPS22HB_ODR_75HZ:
		LPS22HB_settings[0] = controlRegisterValue[0] | LPS22HB_ODR_75HZ;
		break;
	}

	LPS22HB_writeRegister(LPS22HB_CTRL_REG1, LPS22HB_settings);
}

void LPS22HB_reset() {

	LPS22HB_settings[0] = LPS22HB_SWRESET;
	LPS22HB_writeRegister(LPS22HB_CTRL_REG2, LPS22HB_settings);
	HAL_Delay(10);
}

//LPS22HB_checkStatus(uint8_t* statusRegisterValue) {
//	LPS22HB_readRegister(LPS22HB_STATUS, LPS22HB_statusRegisterValue);
//}

void LPS22HB_readPressure(int* pressure) {

	LPS22HB_pressureData tempData;

//	LPS22HB_init();
//	HAL_Delay(10);

	LPS22HB_readRegister(LPS22HB_PRESS_OUT_XL, tempData.LSBpressure);
	LPS22HB_readRegister(LPS22HB_PRESS_OUT_L, tempData.MIDpressure);
	LPS22HB_readRegister(LPS22HB_PRESS_OUT_H, tempData.MSBpressure);

	tempData.LPS22HB_pressureRaw = ((uint32_t) tempData.MSBpressure[0] << 16)
			| (((uint32_t) tempData.MIDpressure[0] << 8)
					| tempData.LSBpressure[0]);
	tempData.LPS22HB_pressureHPa = ((int) tempData.LPS22HB_pressureRaw) / 4096;

	*pressure = tempData.LPS22HB_pressureHPa;

}
void LPS22HB_readTemperature(int* temperature) {

	LPS22HB_tempData tempData;

//	LPS22HB_init();
//	HAL_Delay(10);

	LPS22HB_readRegister(LPS22HB_TEMP_OUT_L, tempData.LSBtemp);
	LPS22HB_readRegister(LPS22HB_TEMP_OUT_H, tempData.MSBtemp);

	tempData.LPS22HB_temperatureRaw = (((uint16_t) tempData.MSBtemp[0] << 8)
			| tempData.LSBtemp[0]);
	tempData.LPS22HB_temperature = ((int) tempData.LPS22HB_temperatureRaw)
			/ 100;

	*temperature = tempData.LPS22HB_temperature;
}

