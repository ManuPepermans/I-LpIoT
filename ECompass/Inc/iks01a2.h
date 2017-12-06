#ifndef IKS01A2_H_
#define IKS01A2_H_

#include <limits.h>
#include <math.h>

//I2C address
#define LSM303_MAG_ADDRESS (0x1e << 1)
#define LSM303_ACC_ADDRESS (0x19 << 1)

//Register map Magnetometer
#define CFG_REG_A_M 0x60
#define CFG_REG_B_M 0x61
#define CFG_REG_C_M 0x62

#define OUTX_L_REG_M 0x68
#define OUTX_H_REG_M 0x69
#define OUTY_L_REG_M 0x6A
#define OUTY_H_REG_M 0x6B
#define OUTZ_L_REG_M 0x6C
#define OUTZ_H_REG_M 0x6D

//Multi-read
#define LSM303_MAG_X_L_A_MULTI_READ (OUTX_L_REG_M | 0x80)
#define LSM303_MAG_Y_L_A_MULTI_READ (OUTY_L_REG_M | 0x80)
#define LSM303_MAG_Z_L_A_MULTI_READ (OUTZ_L_REG_M | 0x80)

//Register map Accelerometer
#define LSM303_ACC_CTRL_REG1_A 0x20
#define LSM303_ACC_CTRL_REG3_A 0x22
#define LSM303_ACC_Z_H_A 0x2D
#define LSM303_ACC_Z_L_A 0x2C
#define LSM303_ACC_X_L_A 0x28

#define LSM303_ACC_Z_L_A_MULTI_READ (LSM303_ACC_Z_L_A | 0x80)
#define LSM303_ACC_X_L_A_MULTI_READ (LSM303_ACC_X_L_A | 0x80)

#endif /* IKS01A2_H_ */
