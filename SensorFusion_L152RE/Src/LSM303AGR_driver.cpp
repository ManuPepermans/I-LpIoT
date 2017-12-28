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

void LSM303AGR_ACC_init() {

	LSM303AGR_settings[0] = LSM303AGR_ACC_ODR_100HZ | LSM303AGR_ACC_X_EN
			| LSM303AGR_ACC_Y_EN | LSM303AGR_ACC_Z_EN;
	LSM303AGR_ACC_writeRegister(LSM303AGR_ACC_CTRL_REG1, LSM303AGR_settings);
	LSM303AGR_settings[0] = LSM303AGR_ACC_I1_DRDY2_EN;
	LSM303AGR_ACC_writeRegister(LSM303AGR_ACC_CTRL_REG3, LSM303AGR_settings);
}
void LSM303AGR_MAG_init() {

	LSM303AGR_settings[0] = LSM303AGR_MAG_MODE_CONT | LSM303AGR_MAG_ODR_10HZ;
	LSM303AGR_MAG_writeRegister(LSM303AGR_MAG_CFG_REG_A, LSM303AGR_settings);
	LSM303AGR_settings[0] = LSM303AGR_MAG_INT_MAG_EN;
	LSM303AGR_MAG_writeRegister(LSM303AGR_MAG_CFG_REG_C, LSM303AGR_settings);

}

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

	Type3Axis16bit_U raw_data_tmp;

	// Normal mode 10-bit, shift = 6 and FS = 2
	u8_t op_mode = 1, fs_mode = 0, shift = 6;

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
	//Type3Axis16bit_U raw_data_tmp;

	/* Read output registers from LSM303AGR_MAG_OUTX_L to LSM303AGR_MAG_OUTZ_H. */
	LSM303AGR_MAG_readRegister(LSM303AGR_MAG_MULTI_READ, regValue);
//	HAL_I2C_Mem_Read(&hi2c1, LSM303AGR_MAG_I2C_ADDRESS, LSM303AGR_MAG_MULTIREAD,
//	I2C_MEMADD_SIZE_8BIT, regValue, sizeof(regValue), HAL_MAX_DELAY);

	regValueInt16 = (int16_t *) regValue;

	/* Format the data. */
	pData[0] = regValueInt16[0];
	pData[1] = regValueInt16[1];
	pData[2] = regValueInt16[2];

}
