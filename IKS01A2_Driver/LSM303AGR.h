/*
 * LSM303AGR.h
 *
 *  Created on: Jan 2, 2018
 *      Author: levim
 */

#ifndef LSM303AGR_H_
#define LSM303AGR_H_

/*LSM303AGR i2c addresses*/
//Transformed in 8-bit form
#define LSM303AGR_ACC_I2C_ADDRESS 0x32
#define LSM303AGR_MAG_I2C_ADDRESS 0x3c

/*LSM303AGR register map*/
//LSM303AGR Accelerometer
#define LSM303AGR_ACC_WHO_AM_I_REG      0x0F
#define LSM303AGR_ACC_CTRL_REG1     	0x20
#define LSM303AGR_ACC_CTRL_REG4    		0x23
#define LSM303AGR_ACC_CTRL_REG5			0x24
#define LSM303AGR_ACC_STATUS_REG   		0x27
#define LSM303AGR_ACC_OUT_X_L   		0x28
#define LSM303AGR_ACC_OUT_X_H   		0x29
#define LSM303AGR_ACC_OUT_Y_L   		0x2A
#define LSM303AGR_ACC_OUT_Y_H   		0x2B
#define LSM303AGR_ACC_OUT_Z_L   		0x2C
#define LSM303AGR_ACC_OUT_Z_H   		0x2D
#define LSM303AGR_ACC_MULTI_READ		(LSM303AGR_ACC_OUT_X_L | 0x80)

//LSM303AGR Magnetometer
#define LSM303AGR_MAG_OFFSET_X_REG_L    0x45
#define LSM303AGR_MAG_OFFSET_X_REG_H    0x46
#define LSM303AGR_MAG_OFFSET_Y_REG_L    0x47
#define LSM303AGR_MAG_OFFSET_Y_REG_H    0x48
#define LSM303AGR_MAG_OFFSET_Z_REG_L    0x49
#define LSM303AGR_MAG_OFFSET_Z_REG_H    0x4A
#define LSM303AGR_MAG_WHO_AM_I      	0x4F
#define LSM303AGR_MAG_CFG_REG_A     	0x60
#define LSM303AGR_MAG_CFG_REG_B     	0x61
#define LSM303AGR_MAG_CFG_REG_C     	0x62
#define LSM303AGR_MAG_STATUS    		0x67
#define LSM303AGR_MAG_OUTX_L    		0x68
#define LSM303AGR_MAG_OUTX_H    		0x69
#define LSM303AGR_MAG_OUTY_L    		0x6A
#define LSM303AGR_MAG_OUTY_H    		0x6B
#define LSM303AGR_MAG_OUTZ_L    		0x6C
#define LSM303AGR_MAG_OUTZ_H    		0x6D
#define LSM303AGR_MAG_MULTI_READ		(LSM303AGR_MAG_OUTX_L | 0x80)

/*LSM303AGR settings*/
//LSM303AGR Accelerometer
//CTRL_REG_1 (0x20)
#define LSM303AGR_ACC_ODR_POWERDOWN		0x00
#define LSM303AGR_ACC_ODR_1HZ			0x10
#define LSM303AGR_ACC_ODR_10HZ			0x20
#define LSM303AGR_ACC_Z_EN				0x04
#define LSM303AGR_ACC_Y_EN				0x02
#define LSM303AGR_ACC_X_EN				0x01
//CTRL_REG_4 (0x23)
#define LSM303AGR_ACC_BDU_EN			0x80
//CTRL_REG5 (0x24)
#define LSM303AGR_ACC_BOOT				0x80

//LSM303AGR Magnetometer
//CFG_REG_A_M (0x60)
#define LSM303AGR_MAG_COMP_TEMP_EN		0x80
#define LSM303AGR_MAG_SOFT_RST			0x20
#define LSM303AGR_MAG_LP_EN				0x10
#define LSM303AGR_MAG_ODR_10HZ			0x00
#define LSM303AGR_MAG_MD_CONT			0x00
#define LSM303AGR_MAG_MD_SIGNLE			0x01
#define LSM303AGR_MAG_MD_IDLE			0x02
//CFG_REG_B_M
#define LSM303AGR_MAG_OFF_CANC_ONE_SHOT	0x10
#define LSM303AGR_MAG_OFF_CANC			0x02
#define LSM303AGR_MAG_LPF				0x01
//CFG_REG_C_M
#define LSM303AGR_MAG_BDU				0x10

/*LPS22HB WHO_AM_I response*/
#define LSM303AGR_ACC_WHO_AM_I_RSP 		0x33
#define LSM303AGR_MAG_WHO_AM_I_RSP 		0x40

typedef union {
	uint8_t registerData[6];
	int16_t rawData[3];
} LSM303AGR_ACC_TEMP_DATA;
int32_t accData[3];

typedef union {
	uint8_t registerData[6];
	int16_t rawData[3];
} LSM303AGR_MAG_TEMP_DATA;

uint8_t LSM303AGR_setting;
I2C_HandleTypeDef *LSM303AGR_hi2c;

void LSM303AGR_setI2CInterface(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef LSM303AGR_readRegister(uint8_t LSM303AGR_reg,
		uint8_t LSM303AGR_data, uint8_t ACC_MAG);
HAL_StatusTypeDef LSM303AGR_writeRegister(uint8_t LSM303AGR_reg,
		uint8_t LSM303AGR_data, uint8_t ACC_MAG);
void LSM303AGR_init();
void LSM303AGR_ACC_reset();
void LSM303AGR_MAG_reset();
void LSM303AGR_powerDownAccelerometer();
void LSM303AGR_powerDownMagnetometer();
void LSM303AGR_wakeUpAccelerometer();
void LSM303AGR_wakeUpMagnetometer();
void LSM303AGR_ACC_readAccelerationData(int32_t *pData);
void LSM303AGR_MAG_readMagneticData(int32_t *pData);
void LSM303AGR_MAG_readMagneticRawData(int16_t *pData);

#endif /* LSM303AGR_H_ */
