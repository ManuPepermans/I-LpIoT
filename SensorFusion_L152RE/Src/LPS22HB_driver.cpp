/********************************************************
 * @author: Mariën Levi
 * @description: A low power driver for the LPS22HB sensor
 ********************************************************/

#include "LPS22HB_driver.h"
#include "stm32l1xx_hal.h"

/**
 * Sets the I2C interface for the sensor
 * @param hi2c [description]
 */
void LPS22HB_setI2CInterface(I2C_HandleTypeDef *hi2c) {
	LPS22HB_hi2c = hi2c;
}

/**
 * Reads the value of a certain register specified by the user
 * @param  reg   [the address of the register]
 * @param  pData [where the data needs to be stored]
 * @return       [status of the I2C transfer {HAL_OK | HAL_Error}]
 */
HAL_I2C_StateTypeDef LPS22HB_readRegister(uint8_t reg, uint8_t pData[]) {

	return HAL_I2C_Mem_Read(LPS22HB_hi2c, LPS22HB_I2C_ADDRESS, reg,
	I2C_MEMADD_SIZE_8BIT, pData, sizeof(pData),
	HAL_MAX_DELAY);
}

/**
 * Writes a given value to a certain register specified by the user
 * @param  reg   [the address of the register]
 * @param  pData [the data that has to be written to the register]
 * @return       [status of the I2C transfer {HAL_OK | HAL_Error}]
 */
HAL_I2C_StateTypeDef LPS22HB_writeRegister(uint8_t reg, uint8_t pData[]) {

	return HAL_I2C_Mem_Write(LPS22HB_hi2c, LPS22HB_I2C_ADDRESS, reg,
	I2C_MEMADD_SIZE_8BIT, pData, sizeof(pData),
	HAL_MAX_DELAY);
}

/**
 * Initialization of the LPS22HB sensor. The sensor is put into powerdown mode
 * (minimal energy consumption). According to the datasheet only the I2C and SPI
 * interfaces keep on running
 */
void LPS22HB_init() {

	LPS22HB_settings[0] = LPS22HB_ODR_POWERDOWN;
	LPS22HB_writeRegister(LPS22HB_CTRL_REG1, LPS22HB_settings);
}

/**
 * Performing of a software reset of the LPS22HB sensor, the following registers
 * are reseted to their default registerValue INTERRUPT_CFG(0Bh), THS_P_L (0Ch),
 * REF_P_H (17h), CTRL_REG1 (10h), CTRL_REG2 (11h), CTRL_REG3 (12h), FIFO_CTRL
 * (14h), REF_P_XL (15h), REF_P_L (16h), REF_P_H (17h) After the reset is
 * performed this bit is self cleared by hardware
 */
void LPS22HB_reset() {

	LPS22HB_settings[0] = LPS22HB_SWRESET;
	LPS22HB_writeRegister(LPS22HB_CTRL_REG2, LPS22HB_settings);
	HAL_Delay(10);
}

/**
 * Manual powerdown of the sensor, a mask is taken so that only the ODR bits are
 * affected
 */
void LPS22HB_powerdown() {
	uint8_t registerValue[1];
	LPS22HB_readRegister(LPS22HB_CTRL_REG1, registerValue);
	//Clears 6th, 5th and 4th bit (powerdown mode)
	registerValue[0] &= ~((1 << 6) | (1 << 5) | (1 << 4));
	LPS22HB_settings[0] = registerValue[0];
	LPS22HB_writeRegister(LPS22HB_CTRL_REG1, LPS22HB_settings);
}

/**
 * Sets the LPS22HB sensor in one shot mode, this is only possible if the sensor
 * was previous in powerdown mode. This triggers a single measurement of
 * pressure and temperature. Once the measurement is done, the bit is self
 * cleared and the sensor is returned in powerdown mode.
 */
void LPS22HB_setOneShotMode() {
	uint8_t registerValue[1];
	LPS22HB_readRegister(LPS22HB_CTRL_REG1, registerValue);
	uint8_t mask = 0b01110000;
	registerValue[0] &= ~mask;
	LPS22HB_settings[0] = registerValue[0] | LPS22HB_ODR_POWERDOWN;
	LPS22HB_writeRegister(LPS22HB_CTRL_REG1, LPS22HB_settings);
	LPS22HB_settings[0] = LPS22HB_ONESHOT;
	LPS22HB_writeRegister(LPS22HB_CTRL_REG2, LPS22HB_settings);
}

/**
 * Sets the LPS22HB sensor in low current mode
 */
void LPS22HB_setLowCurrentMode() {
}

/**
 * Sets the LPS22HB sensor in a certain ODR mode specified by the user
 * @param ODR [The ODR value]
 */
void LPS22HB_setODR(uint8_t ODR) {
	uint8_t registerValue[1];
	LPS22HB_readRegister(LPS22HB_CTRL_REG1, registerValue);
	uint8_t mask = 0b01110000;
	registerValue[0] &= ~mask;
	switch (ODR) {
	case (uint8_t) LPS22HB_ODR_1HZ:
		LPS22HB_settings[0] = registerValue[0] | LPS22HB_ODR_1HZ;
		break;
	case (uint8_t) LPS22HB_ODR_10HZ:
		LPS22HB_settings[0] = registerValue[0] | LPS22HB_ODR_10HZ;
		break;
	case (uint8_t) LPS22HB_ODR_25HZ:
		LPS22HB_settings[0] = registerValue[0] | LPS22HB_ODR_25HZ;
		break;
	case (uint8_t) LPS22HB_ODR_50HZ:
		LPS22HB_settings[0] = registerValue[0] | LPS22HB_ODR_50HZ;
		break;
	case (uint8_t) LPS22HB_ODR_75HZ:
		LPS22HB_settings[0] = registerValue[0] | LPS22HB_ODR_75HZ;
		break;
	}

	LPS22HB_writeRegister(LPS22HB_CTRL_REG1, LPS22HB_settings);
}

/**
 * Gives the current ODR mode of the LPS22HB sensor
 * @return [the current ODR mode]
 */
uint8_t LPS22HB_getODR() {
	return 0;
}

void LPS22HB_checkStatus(uint8_t *statusRegisterValue) {
	LPS22HB_readRegister(LPS22HB_STATUS, LPS22HB_statusRegisterValue);
}

/**
 * Calculates the temperature (°C) and pressure (hPa)
 * @param pressure    [a pointer to the calculated pressure]
 * @param temperature [a pointer to the calculated temperature]
 */
void LPS22HB_getPressureAndTemperature(int *pressure, int *temperature) {

	LPS22HB_pressureData data;
	LPS22HB_temperatureData tempTData;

	LPS22HB_setOneShotMode();

	LPS22HB_readRegister(LPS22HB_PRESS_OUT_XL, data.LSBpressure);
	LPS22HB_readRegister(LPS22HB_PRESS_OUT_L, data.MIDpressure);
	LPS22HB_readRegister(LPS22HB_PRESS_OUT_H, data.MSBpressure);
	LPS22HB_readRegister(LPS22HB_TEMP_OUT_L, tempTData.LSBtemp);
	LPS22HB_readRegister(LPS22HB_TEMP_OUT_H, tempTData.MSBtemp);

	data.LPS22HB_pressureRaw = ((uint32_t) data.MSBpressure[0] << 16)
			| (((uint32_t) data.MIDpressure[0] << 8) | data.LSBpressure[0]);
	data.LPS22HB_pressureHPa = ((int) data.LPS22HB_pressureRaw) / 4096;

	*pressure = data.LPS22HB_pressureHPa;

	LPS22HB_readRegister(LPS22HB_TEMP_OUT_L, tempTData.LSBtemp);
	LPS22HB_readRegister(LPS22HB_TEMP_OUT_H, tempTData.MSBtemp);

	tempTData.LPS22HB_temperatureRaw = (((uint16_t) tempTData.MSBtemp[0] << 8)
			| tempTData.LSBtemp[0]);
	tempTData.LPS22HB_temperature = ((int) tempTData.LPS22HB_temperatureRaw)
			/ 100;

	*temperature = tempTData.LPS22HB_temperature;
}
