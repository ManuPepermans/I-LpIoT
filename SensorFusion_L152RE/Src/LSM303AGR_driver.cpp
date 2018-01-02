/*
 * LSM303AGR_driver.cpp
 *
 *  Created on: 20 dec. 2017
 *      Author: levim
 */

#include "LSM303AGR_driver.h"
#include "stm32l1xx_hal.h"

void LSM303AGR_setI2CInterface(I2C_HandleTypeDef *hi2c) {

	LSM303AGR_hi2c = hi2c;
}

HAL_I2C_StateTypeDef LSM303AGR_ACC_readRegister(uint8_t reg, uint8_t pData[]) {

	return HAL_I2C_Mem_Read(LSM303AGR_hi2c, LSM303AGR_ACC_I2C_ADDRESS, reg,
	I2C_MEMADD_SIZE_8BIT, pData, sizeof(pData), HAL_MAX_DELAY);
}

HAL_I2C_StateTypeDef LSM303AGR_MAG_readRegister(uint8_t reg, uint8_t pData[]) {

	return HAL_I2C_Mem_Read(LSM303AGR_hi2c, LSM303AGR_MAG_I2C_ADDRESS, reg,
	I2C_MEMADD_SIZE_8BIT, pData, sizeof(pData), HAL_MAX_DELAY);

}

HAL_I2C_StateTypeDef LSM303AGR_ACC_writeRegister(uint8_t reg, uint8_t pData[]) {

	return HAL_I2C_Mem_Write(LSM303AGR_hi2c, LSM303AGR_ACC_I2C_ADDRESS, reg,
	I2C_MEMADD_SIZE_8BIT, pData, sizeof(pData), HAL_MAX_DELAY);

}

HAL_I2C_StateTypeDef LSM303AGR_MAG_writeRegister(uint8_t reg, uint8_t pData[]) {

	return HAL_I2C_Mem_Write(LSM303AGR_hi2c, LSM303AGR_MAG_I2C_ADDRESS, reg,
	I2C_MEMADD_SIZE_8BIT, pData, sizeof(pData), HAL_MAX_DELAY);

}

void LSM303AGR_init() {

	//Configure the LSM303AGR Magnetometer
	//10Hz High resolution and continious mode
	uint8_t settings = 0x00;
	HAL_I2C_Mem_Write(LSM303AGR_hi2c, LSM303AGR_MAG_I2C_ADDRESS,
	LSM303AGR_MAG_CFG_REG_A, 1, &settings, 1, 100);


	//Wait 5ms so that the Accelerometer can downloads it's calibration coefficients from
	//the embedded flash to the internal registers
	//Hal_Delay(5);

	//Enables the data-ready interrupt
	//LSM303AGR_settings[0] = LSM303AGR_ACC_I1_DRDY2_EN;
	//LSM303AGR_ACC_writeRegister(LSM303AGR_ACC_CTRL_REG3, LSM303AGR_settings);
	//Enables the X, Y and Z axes and put the accelerometer in powerdown
//	LSM303AGR_settings[0] = LSM303AGR_ACC_X_EN | LSM303AGR_ACC_Y_EN
//			| LSM303AGR_ACC_Z_EN;
//	LSM303AGR_ACC_writeRegister(LSM303AGR_ACC_CTRL_REG1, LSM303AGR_settings);
//
//	LSM303AGR_settings[0] = LSM303AGR_MAG_MODE_CONT | LSM303AGR_MAG_ODR_10HZ;
//	LSM303AGR_MAG_writeRegister(LSM303AGR_MAG_CFG_REG_A, LSM303AGR_settings);
//	LSM303AGR_settings[0] = LSM303AGR_MAG_INT_MAG_EN;
//	LSM303AGR_MAG_writeRegister(LSM303AGR_MAG_CFG_REG_C, LSM303AGR_settings);

}

void LSM303AGR_MAG_readMagneticData(int32_t *pData) {
	int16_t pDataRaw[3];
	float sensitivity = 1.5;

	/* Read raw data from LSM303AGR output register. */
	get_m_axes_raw(pDataRaw);

	/* Calculate the data. */
	pData[0] = (int32_t) (pDataRaw[0] * sensitivity);
	pData[1] = (int32_t) (pDataRaw[1] * sensitivity);
	pData[2] = (int32_t) (pDataRaw[2] * sensitivity);

}

void LSM303AGR_reset() {

}
//void LSM303AGR_powerdown() {
//	uint8_t registerValue[2];
//	LSM303AGR_ACC_readRegister(LSM303AGR_ACC_CTRL_REG1, registerValue[0]);
//	LSM303AGR_MAG_readRegister(LSM303AGR_MAG_CFG_REG_A, registerValue[1]);
//	uint8_t mask = 0xF0;
//	//Clear the 7th, 6th, 5th and 4th bit (powerdown mode)
//	registerValue[0] &= (~mask);
//	//Sets the 1st bit and clears the 0th bit (idle mode)
//	//registerValue[1] = (registerValue[1] & ~())~((1 << 1) | (1 << 0));
//
//	LPS22HB_readRegister(LPS22HB_CTRL_REG1, registerValue);
//	//Clears 6th, 5th and 4th bit (powerdown mode)
//	registerValue[0] &= ~((1 << 6) | (1 << 5) | (1 << 4));
//	LPS22HB_settings[0] = registerValue[0];
//	LPS22HB_writeRegister(LPS22HB_CTRL_REG1, LPS22HB_settings);
//}

/**
 * @brief  Read data from LSM303AGR Accelerometer
 * @param  pData the pointer where the accelerometer data are stored
 * @retval 0 in case of success, an error code otherwise
 */
void get_x_axes(int32_t *pData) {

	int data[3];

	/* Read data from LSM303AGR. */
	LSM303AGR_ACC_Get_Acceleration(data);

	/* Calculate the data. */
	pData[0] = (int32_t) data[0];
	pData[1] = (int32_t) data[1];
	pData[2] = (int32_t) data[2];

}

/*
 * Following is the table of sensitivity values for each case.
 * Values are espressed in ug/digit.
 */
void LSM303AGR_ACC_Get_Acceleration(int *buff) {

	LSM303AGR_ACC_TEMP_DATA raw_data_tmp;

	// Normal mode 10-bit, shift = 6 and FS = 2
	unsigned char op_mode = 1, fs_mode = 0, shift = 6;

	/* Read out raw accelerometer samples */
	LSM303AGR_ACC_readRegister(LSM303AGR_ACC_MULTI_READ, raw_data_tmp.u8bit);
//	HAL_I2C_Mem_Read(&hi2c1, LSM303AGR_ACC_I2C_ADDRESS, LSM303AGR_ACC_MULTIREAD,
//	I2C_MEMADD_SIZE_8BIT, raw_data_tmp.u8bit, sizeof(raw_data_tmp.u8bit),
//	HAL_MAX_DELAY);

	raw_data_tmp.i16bit[0] = (raw_data_tmp.u8bit[1] << 8)
			| raw_data_tmp.u8bit[0];
	raw_data_tmp.i16bit[1] = (raw_data_tmp.u8bit[3] << 8)
			| raw_data_tmp.u8bit[2];
	raw_data_tmp.i16bit[2] = (raw_data_tmp.u8bit[5] << 8)
			| raw_data_tmp.u8bit[4];

	/* Apply proper shift and sensitivity */
	buff[0] = ((raw_data_tmp.i16bit[0] >> shift)
			* LSM303AGR_ACC_Sensitivity_List[op_mode][fs_mode] + 500) / 1000;
	buff[1] = ((raw_data_tmp.i16bit[1] >> shift)
			* LSM303AGR_ACC_Sensitivity_List[op_mode][fs_mode] + 500) / 1000;
	buff[2] = ((raw_data_tmp.i16bit[2] >> shift)
			* LSM303AGR_ACC_Sensitivity_List[op_mode][fs_mode] + 500) / 1000;

}

/**
 * @brief  Read raw data from LSM303AGR Magnetometer
 * @param  pData the pointer where the magnetomer raw data are stored
 * @retval 0 in case of success, an error code otherwise
 */
void get_m_axes(int32_t *pData) {

	int16_t pDataRaw[3];
	float sensitivity = 1.5;

	/* Read raw data from LSM303AGR output register. */
	get_m_axes_raw(pDataRaw);

	/* Calculate the data. */
	pData[0] = (int32_t) (pDataRaw[0] * sensitivity);
	pData[1] = (int32_t) (pDataRaw[1] * sensitivity);
	pData[2] = (int32_t) (pDataRaw[2] * sensitivity);

}

/**
 * @brief  Read raw data from LSM303AGR Magnetometer
 * @param  pData the pointer where the magnetomer raw data are stored
 * @retval 0 in case of success, an error code otherwise
 */
void get_m_axes_raw(int16_t *pData) {

	uint8_t regValue[6] = { 0, 0, 0, 0, 0, 0 };
	int16_t *regValueInt16;

	/* Read output registers from LSM303AGR_MAG_OUTX_L to LSM303AGR_MAG_OUTZ_H. */
	LSM303AGR_MAG_readRegister(LSM303AGR_MAG_MULTI_READ, regValue);

	regValueInt16 = (int16_t *) regValue;

	/* Format the data. */
	pData[0] = regValueInt16[0];
	pData[1] = regValueInt16[1];
	pData[2] = regValueInt16[2];

}
