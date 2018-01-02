#include "LSM303AGR.h"
#include "stm32l1xx_hal.h"

void LSM303AGR_setI2CInterface(I2C_HandleTypeDef *hi2c) {

	LSM303AGR_hi2c = hi2c;
}

void LSM303AGR_init() {

	LSM303AGR_setting = LSM303AGR_ACC_ODR_1HZ | LSM303AGR_ACC_X_EN
			| LSM303AGR_ACC_Y_EN | LSM303AGR_ACC_Z_EN;
	HAL_I2C_Mem_Write(LSM303AGR_hi2c, LSM303AGR_ACC_I2C_ADDRESS,
	LSM303AGR_ACC_CTRL_REG1, I2C_MEMADD_SIZE_8BIT, &LSM303AGR_setting,
			sizeof(LSM303AGR_setting), HAL_MAX_DELAY);
	LSM303AGR_setting = LSM303AGR_ACC_BDU_EN;
	HAL_I2C_Mem_Write(LSM303AGR_hi2c, LSM303AGR_ACC_I2C_ADDRESS,
	LSM303AGR_ACC_CTRL_REG4, I2C_MEMADD_SIZE_8BIT, &LSM303AGR_setting,
			sizeof(LSM303AGR_setting), HAL_MAX_DELAY);

	LSM303AGR_setting = LSM303AGR_MAG_ODR_10HZ | LSM303AGR_MAG_COMP_TEMP_EN;
	HAL_I2C_Mem_Write(LSM303AGR_hi2c, LSM303AGR_MAG_I2C_ADDRESS,
	LSM303AGR_MAG_CFG_REG_A, I2C_MEMADD_SIZE_8BIT, &LSM303AGR_setting,
			sizeof(LSM303AGR_setting), HAL_MAX_DELAY);
}

void LSM303AGR_reset() {

}
void LSM303AGR_powerdown() {

}

void LSM303AGR_ACC_readAccelerationData(int32_t *pData) {

	LSM303AGR_ACC_TEMP_DATA rawData;

	HAL_I2C_Mem_Read(LSM303AGR_hi2c, LSM303AGR_ACC_I2C_ADDRESS,
	LSM303AGR_ACC_MULTI_READ,
	I2C_MEMADD_SIZE_8BIT, rawData.registerData, sizeof(rawData.registerData),
	HAL_MAX_DELAY);

	rawData.rawData[0] = (rawData.registerData[1] << 8)
			| rawData.registerData[0];
	rawData.rawData[1] = (rawData.registerData[3] << 8)
			| rawData.registerData[2];
	rawData.rawData[2] = (rawData.registerData[5] << 8)
			| rawData.registerData[4];

	/* Apply proper shift and sensitivity */
	// Normal mode 10-bit, shift = 6 and FS = 2
	accelerationData[0] = (int32_t) (((rawData.rawData[0] >> 6) * 3900 + 500)
			/ 1000);
	accelerationData[1] = (int32_t) (((rawData.rawData[1] >> 6) * 3900 + 500)
			/ 1000);
	accelerationData[2] = (int32_t) (((rawData.rawData[2] >> 6) * 3900 + 500)
			/ 1000);

	pData[0] = accelerationData[0];
	pData[1] = accelerationData[1];
	pData[2] = accelerationData[2];
}

/**
 * @brief  Read raw data from LSM303AGR Magnetometer
 * @param  pData the pointer where the magnetomer raw data are stored
 * @retval 0 in case of success, an error code otherwise
 */
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
	magnetometerData[0] = (int32_t) (rawData.rawData[0] * 1.5f);
	magnetometerData[1] = (int32_t) (rawData.rawData[1] * 1.5f);
	magnetometerData[2] = (int32_t) (rawData.rawData[2] * 1.5f);

	pData[0] = magnetometerData[0];
	pData[1] = magnetometerData[1];
	pData[2] = magnetometerData[2];

}

//void LSM303AGR_algorithm() {
//
//	ecompassData data;
//
//	data.roll_rad = atan2((float) accelerationData[1], accelerationData[2]);
//	data.roll = data.roll_rad * 57.296;
//
//	data.temp[0] = (float) accelerationData[1] * sin(data.roll_rad);
//	data.temp[1] = cos(data.roll_rad) * (float) accelerationData[2]
//			+ data.temp[0];
//	data.pitch_rad = -atan2((float) accelerationData[1],
//			sqrt(
//					pow((float) accelerationData[1], 2)
//							+ pow((float) accelerationData[2], 2)));
//	data.pitch = data.pitch_rad * 57.296;
//
//	//Determining where the head of the compass is pointed at
//	data.temp[0] = magnetometerData[2] * sin(data.roll_rad)
//			- magnetometerData[1] * cos(data.roll_rad);
//	data.temp[1] = magnetometerData[1] * sin(data.roll_rad)
//			+ magnetometerData[2] * cos(data.roll_rad);
//	data.temp[2] = magnetometerData[0] * cos(data.pitch_rad)
//			+ data.temp[1] * sin(data.pitch_rad);
//	data.yaw_rad = atan2((float) data.temp[0], data.temp[2]);
//	data.degree = data.yaw_rad * 57.296; //180/PI
//	data.degree = data.degree + 50;
//
//	//Transforming degrees into a wind direction
//	if (data.degree > 360) {
//		data.degree = data.degree - 360;
//	}
//	if (data.degree < 0) {
//		data.degree = data.degree + 360;
//	}
//
//	degreeInt = (int16_t) data.degree;
//
//}

//void LSM303AGR_algorithm() {
//
//	LSM303AGR_ACC_readAccelerationData();
//	LSM303AGR_MAG_readMagneticData();
//
//	ecompassData data;
//
//	data.axes_aF[0] = (float) accelerationData[0];
//	data.axes_aF[1] = (float) accelerationData[1];
//	data.axes_aF[2] = (float) accelerationData[2];
//
//	data.axes_aT[0] = (data.axes_aF[0] * 2 / 128);
//	data.axes_aT[1] = (data.axes_aF[1] * 2 / 128);
//	data.axes_aT[2] = (data.axes_aF[2] * 2 / 128);
//
//	// Some math to get the thing working
//	data.roll = atan2f(data.axes_aT[1], data.axes_aT[2]);
//	data.pitch = atan2f(-data.axes_aT[0],
//			data.axes_aT[1] * sinf(data.roll)
//					+ data.axes_aT[2] * cosf(data.roll));
//
//	data.x = magnetometerData[0] * cosf(data.pitch)
//			+ (magnetometerData[1] * sinf(data.roll)
//					+ magnetometerData[2] * cosf(data.roll)) * sinf(data.pitch);
//	data.y = magnetometerData[2] * sinf(data.roll)
//			- magnetometerData[1] * cosf(data.roll);
//
//	data.h = atan2f(data.y, data.x);
//	data.dir = data.h * (180 / 3.1415);
//	if (data.dir < 0)
//		data.dir += 360;
//
//}
