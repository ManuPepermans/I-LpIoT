/**
 * LSM303AGR_driver.h
 * Purpose: A low power driver for the LSM303AGR sensor
 *
 * @author: Levi Mariën
 * @version 1.0 11/12/2017
 */

#include "stm32l1xx_hal.h"
#include "math.h"
#ifndef LSM303AGR_DRIVER_H_
#define LSM303AGR_DRIVER_H_

/*LSM303AGR i2c addresses*/
//Transformed in 8-bit form
#define LSM303AGR_ACC_I2C_ADDRESS 0x32
#define LSM303AGR_MAG_I2C_ADDRESS 0x3c

/*LSM303AGR register map*/
//LSM303AGR Accelerometer
#define LSM303AGR_ACC_STATUS_REG_AUX_A 	0x07
#define LSM303AGR_ACC_OUT_TEMP_L   		0x0C
#define LSM303AGR_ACC_OUT_TEMP_H   		0x0D
#define LSM303AGR_ACC_INT_COUNTER_REG   0x0E
#define LSM303AGR_ACC_WHO_AM_I_REG      0x0F
#define LSM303AGR_ACC_TEMP_CFG_REG      0x1F
#define LSM303AGR_ACC_CTRL_REG1     	0x20
#define LSM303AGR_ACC_CTRL_REG2     	0x21
#define LSM303AGR_ACC_CTRL_REG3     	0x22
#define LSM303AGR_ACC_CTRL_REG4    		0x23
#define LSM303AGR_ACC_CTRL_REG5     	0x24
#define LSM303AGR_ACC_CTRL_REG6     	0x25
#define LSM303AGR_ACC_REFERENCE     	0x26
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

/*LPS22HB WHO_AM_I response*/
#define LSM303AGR_ACC_WHO_AM_I_RSP 		0x33
#define LSM303AGR_MAG_WHO_AM_I_RSP 		0x40

/* Datatype definition */
typedef short int i16_t;
typedef unsigned char u8_t;
typedef union {
	i16_t i16bit[3];
	u8_t u8bit[6];
} Type3Axis16bit_U;

#define X 0
#define Y 1
#define Z 2

int32_t axes_a[3];
int32_t axes_m[3];
float roll = 0, pitch = 0, roll_rad = 0, pitch_rad = 0;
float yaw_rad = 0, degree = 0, godr = 0;
float temp[3] = { }, hardCor[3] = { };
char buff[3];
uint16_t degree_int = 0;

uint8_t LSM303AGR_settings[1];
uint8_t LSM303AGR_statusRegisterValue[1];

I2C_HandleTypeDef *LSM303AGR_hi2c;

/*
 * Following is the table of sensitivity values for each case.
 * Values are espressed in ug/digit.
 */
const long long LSM303AGR_ACC_Sensitivity_List[3][4] = {
/* HR 12-bit */
{ 980, /* FS @2g */
1950, /* FS @4g */
3900, /* FS @8g */
11720, /* FS @16g */
},

/* Normal 10-bit */
{ 3900, /* FS @2g */
7820, /* FS @4g */
15630, /* FS @8g */
46900, /* FS @16g */
},

/* LP 8-bit */
{ 15630, /* FS @2g */
31260, /* FS @4g */
62520, /* FS @8g */
187580, /* FS @16g */
}, };

/*LSM303AGR settings*/
//LSM303AGR Accelerometer
#define LSM303AGR_ACC_TEMP_EN			0x00
#define LSM303AGR_ACC_TEMP_DIS			0xC0
#define LSM303AGR_ACC_ODR_POWERDOWN		0x00
#define LSM303AGR_ACC_ODR_1HZ			0x10 //HR/NORMAL/LP
#define LSM303AGR_ACC_ODR_10HZ			0x20 //HR/NORMAL/LP
#define LSM303AGR_ACC_ODR_25HZ			0x30 //HR/NORMAL/LP
#define LSM303AGR_ACC_ODR_50HZ			0x40 //HR/NORMAL/LP
#define LSM303AGR_ACC_ODR_100HZ			0x50 //HR/NORMAL/LP
#define LSM303AGR_ACC_ODR_200HZ			0x60 //HR/NORMAL/LP
#define LSM303AGR_ACC_ODR_400HZ			0x70 //HR/NORMAL/LP
#define LSM303AGR_ACC_ODR_1620HZ		0x80 //LP
#define LSM303AGR_ACC_ODR_1344HZ		0x90 //HR/NORMAL
#define LSM303AGR_ACC_ODR_5376HZ		0x90 //LP
#define LSM303AGR_ACC_LP_DIS			0x00 //HR is set to 0 --> Normal mode (10-bit data output)
#define LSM303AGR_ACC_LP_EN				0x08 //HR is set to 0 --> LP mode (8-bit data output)
#define LSM303AGR_ACC_Z_DIS				0x00
#define LSM303AGR_ACC_Z_EN				0x04
#define LSM303AGR_ACC_Y_DIS				0x00
#define LSM303AGR_ACC_Y_EN				0x02
#define LSM303AGR_ACC_X_DIS				0x00
#define LSM303AGR_ACC_X_EN				0x01
#define LSM303AGR_ACC_I1_DRDY1_DIS		0x00
#define LSM303AGR_ACC_I1_DRDY1_EN		0x10
#define LSM303AGR_ACC_I1_DRDY2_DIS		0x00
#define LSM303AGR_ACC_I1_DRDY2_EN		0x08
#define LSM303AGR_ACC_BDU_CONT			0x00
#define LSM303AGR_ACC_BDU_NONCONT		0x80
#define LSM303AGR_ACC_FS_2G				0x00
#define LSM303AGR_ACC_FS_4G				0x10
#define LSM303AGR_ACC_FS_8G				0x20
#define LSM303AGR_ACC_FS_16G			0x30
#define LSM303AGR_ACC_HR_DIS			0x00
#define LSM303AGR_ACC_HR_EN				0x08 //LP is set to 1 --> High resolution mode (12-bit data output)
#define LSM303AGR_ACC_BOOT_NORMAL		0x00
#define LSM303AGR_ACC_BOOT_REBOOT		0x80

//LSM303AGR Magnetometer
#define LSM303AGR_MAG_COMP_TEMP_DIS		0x00
#define LSM303AGR_MAG_COMP_TEMP_EN		0x80
#define LSM303AGR_MAG_REBOOT_DIS		0x00
#define LSM303AGR_MAG_REBOOT_EN			0x40
#define LSM303AGR_MAG_SOFT_RST			0x20
#define LSM303AGR_MAG_LP_DIS			0x00
#define LSM303AGR_MAG_LP_EN				0x10
#define LSM303AGR_MAG_ODR_10HZ			0x00
#define LSM303AGR_MAG_ODR_20HZ			0x04
#define LSM303AGR_MAG_ODR_50HZ			0x08
#define LSM303AGR_MAG_ODR_100HZ			0x0C
#define LSM303AGR_MAG_MODE_CONT			0x00
#define LSM303AGR_MAG_MODE_SINGLE		0x01
#define LSM303AGR_MAG_MODE_IDLE1		0x02
#define LSM303AGR_MAG_MODE_IDLE2		0x03
#define LSM303AGR_MAG_OFF_CANC_ON_SHOT_DIS	0x00
#define LSM303AGR_MAG_OFF_CANC_ON_SHOT_EN	0x10
#define LSM303AGR_MAG_INT_MAG_DIS		0x00
#define LSM303AGR_MAG_INT_MAG_EN		0x01

#define LSM303AGR_
/*Function prototypes for the LPS22HB*/
void LSM303AGR_setI2CInterface(I2C_HandleTypeDef *hi2c);
HAL_I2C_StateTypeDef LSM303AGR_ACC_readRegister(uint8_t reg, uint8_t pData[]);
HAL_I2C_StateTypeDef LSM303AGR_MAG_readRegister(uint8_t reg, uint8_t pData[]);
HAL_I2C_StateTypeDef LSM303AGR_ACC_writeRegister(uint8_t reg, uint8_t pData[]);
HAL_I2C_StateTypeDef LSM303AGR_MAG_writeRegister(uint8_t reg, uint8_t pData[]);
void LSM303AGR_ACC_init();
void LSM303AGR_MAG_init();
void LSM303AGR_ACC_readAccelerationData();
void LSM303AGR_MAG_readMagneticData();
void get_x_axes(int32_t *pData);
void LSM303AGR_ACC_Get_Acceleration(int *buff);
void get_m_axes(int32_t *pData);
void get_m_axes_raw(int16_t *pData);

#endif /* LSM303AGR_DRIVER_H_ */
