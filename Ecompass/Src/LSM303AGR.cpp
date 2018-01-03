#include "LSM303AGR.h"
#include "stm32l1xx_hal.h"

void LSM303AGR_setI2CInterface(I2C_HandleTypeDef *hi2c) {

	LSM303AGR_hi2c = hi2c;
}

HAL_StatusTypeDef LSM303AGR_readRegister(uint8_t LSM303AGR_reg,
		uint8_t* LSM303AGR_data, uint8_t ACC_MAG) {

	if (ACC_MAG == 0) {
		HAL_I2C_Mem_Read(LSM303AGR_hi2c, LSM303AGR_ACC_I2C_ADDRESS,
				LSM303AGR_reg, I2C_MEMADD_SIZE_8BIT, LSM303AGR_data,
				sizeof(LSM303AGR_data), HAL_MAX_DELAY);
	} else {
		HAL_I2C_Mem_Read(LSM303AGR_hi2c, LSM303AGR_MAG_I2C_ADDRESS,
				LSM303AGR_reg, I2C_MEMADD_SIZE_8BIT, LSM303AGR_data,
				sizeof(LSM303AGR_data), HAL_MAX_DELAY);
	}
}

HAL_StatusTypeDef LSM303AGR_writeRegister(uint8_t LSM303AGR_reg,
		uint8_t* LSM303AGR_data, uint8_t ACC_MAG) {

	if (ACC_MAG == 0) {
		HAL_I2C_Mem_Write(LSM303AGR_hi2c, LSM303AGR_ACC_I2C_ADDRESS,
				LSM303AGR_reg, I2C_MEMADD_SIZE_8BIT, LSM303AGR_data,
				sizeof(LSM303AGR_data), HAL_MAX_DELAY);
	} else {
		HAL_I2C_Mem_Write(LSM303AGR_hi2c, LSM303AGR_MAG_I2C_ADDRESS,
				LSM303AGR_reg, I2C_MEMADD_SIZE_8BIT, LSM303AGR_data,
				sizeof(LSM303AGR_setting), HAL_MAX_DELAY);
	}

}

void LSM303AGR_init() {

	LSM303AGR_setting = LSM303AGR_ACC_ODR_1HZ | LSM303AGR_ACC_X_EN
			| LSM303AGR_ACC_Y_EN | LSM303AGR_ACC_Z_EN;
	LSM303AGR_writeRegister(LSM303AGR_ACC_CTRL_REG1, &LSM303AGR_setting, 0);

	LSM303AGR_setting = LSM303AGR_ACC_BDU_EN;
	LSM303AGR_writeRegister(LSM303AGR_ACC_CTRL_REG4, &LSM303AGR_setting, 0);

	LSM303AGR_setting = LSM303AGR_MAG_COMP_TEMP_EN | LSM303AGR_MAG_LP_EN
			| LSM303AGR_MAG_ODR_10HZ;
	LSM303AGR_writeRegister(LSM303AGR_MAG_CFG_REG_A, &LSM303AGR_setting, 1);

	LSM303AGR_setting = LSM303AGR_MAG_OFF_CANC | LSM303AGR_MAG_LPF;
	LSM303AGR_writeRegister(LSM303AGR_MAG_CFG_REG_B, &LSM303AGR_setting, 1);

	LSM303AGR_setting = LSM303AGR_MAG_BDU;
	LSM303AGR_writeRegister(LSM303AGR_MAG_CFG_REG_C, &LSM303AGR_setting, 1);

}

void LSM303AGR_ACC_reset() {

	LSM303AGR_setting = LSM303AGR_ACC_BOOT;
	LSM303AGR_writeRegister(LSM303AGR_ACC_CTRL_REG5, &LSM303AGR_setting, 0);

}

void LSM303AGR_MAG_reset() {

	LSM303AGR_setting = LSM303AGR_MAG_SOFT_RST;
	LSM303AGR_writeRegister(LSM303AGR_MAG_CFG_REG_A, &LSM303AGR_setting, 1);

}

void LSM303AGR_powerdown() {

}

void LSM303AGR_ACC_readAccelerationData(int32_t *pData) {

	LSM303AGR_ACC_TEMP_DATA rawData;

	LSM303AGR_readRegister(LSM303AGR_ACC_MULTI_READ, rawData.registerData, 0);

//	HAL_I2C_Mem_Read(LSM303AGR_hi2c, LSM303AGR_ACC_I2C_ADDRESS,
//	LSM303AGR_ACC_MULTI_READ,
//	I2C_MEMADD_SIZE_8BIT, rawData.registerData, sizeof(rawData.registerData),
//	HAL_MAX_DELAY);

	rawData.rawData[0] = (rawData.registerData[1] << 8)
			| rawData.registerData[0];
	rawData.rawData[1] = (rawData.registerData[3] << 8)
			| rawData.registerData[2];
	rawData.rawData[2] = (rawData.registerData[5] << 8)
			| rawData.registerData[4];

	/* Apply proper shift and sensitivity */
	// Normal mode 10-bit, shift = 6 and FS = 2
	pData[0] = (int32_t) (((rawData.rawData[0] >> 6) * 3900 + 500) / 1000);
	pData[1] = (int32_t) (((rawData.rawData[1] >> 6) * 3900 + 500) / 1000);
	pData[2] = (int32_t) (((rawData.rawData[2] >> 6) * 3900 + 500) / 1000);

}

void LSM303AGR_MAG_readMagneticData(int32_t *pData) {

	LSM303AGR_MAG_TEMP_DATA rawData;

	HAL_I2C_Mem_Read(LSM303AGR_hi2c, LSM303AGR_MAG_I2C_ADDRESS,
	LSM303AGR_MAG_MULTI_READ,
	I2C_MEMADD_SIZE_8BIT, rawData.registerData, sizeof(rawData.registerData),
	HAL_MAX_DELAY);

	rawData.rawData[0] = (rawData.registerData[1] << 8)
			| rawData.registerData[0];
	rawData.rawData[1] = (rawData.registerData[3] << 8)
			| rawData.registerData[2];
	rawData.rawData[2] = (rawData.registerData[5] << 8)
			| rawData.registerData[4];

	/* Calculate the data. */
	pData[0] = (int32_t) (rawData.rawData[0] * 1.5f);
	pData[1] = (int32_t) (rawData.rawData[1] * 1.5f);
	pData[2] = (int32_t) (rawData.rawData[2] * 1.5f);

}
