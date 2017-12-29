
#ifndef __LM303_H
#define __LM303_H
#include "stm32l1xx_hal.h"
#include "main.h"


//Register
#define LSM303_ACC_ADDRESS (0x19 << 1) // ADRESS from the accelero
#define LSM303_MAG_ADDRESS 0x3C // ADRESS from the magnetometer
#define LPS22HB_BARO_ADDRESS 0xBA // ADRESS from the barometer  "10111010"

#define LPS22HB_AWHO_AM_I 0x0F
#define LPS22HB_CTRL_REG2 0x11
#define LPS22HB_PRESS_OUT_XL 0x28	// Pressure output value (LSB)
#define LPS22HB_OUT_L 0x29			// Pressure output value (mid part)
#define LPS22HB_OUT_H 0x2A			// Pressure output value (MSB)
#define LPS22HB_TEMP_OUT_L 0x2B		// Temperature output value (LSB)
#define LPS22HB_TEMP_OUT_H 0x2C		// Temperature output value (MSB)


#define LSM303_ACC_CTRL_REG1_A 0x20 // Control register
#define LSM303_ACC_CTRL_REG1_A 0x20 // Control register
#define LSM303_ACC_CTRL_REG3_A 0x22
#define LSM303_MAG_WHO_AM_I 0x4F
#define LSM303_CFG_REG_A_M 0x60
#define LSM303_CFG_REG_C_M 0x62
#define LSM303_ACC_Z_H_A 0x2D // Z
#define LSM303_ACC_Z_L_A 0x2C //  Z
#define LSM303_ACC_X_L_A 0x28 // X

#define OUTX_L_REG_M 0x68
#define OUTX_H_REG_M 0x69
#define OUTY_L_REG_M 0x6A
#define OUTY_H_REG_M 0x6B
#define OUTZ_L_REG_M 0x6C
#define OUTZ_H_REG_M 0x6D

//Set one shot mode for Barometer

#define LPS22HB_ONE__SHOT_ENABLE 0x11 // 0001 0001

#define LSM303_SETTINGS1 0x00 // Mag = 10 Hz (HR and continuous mode) //This has to be set to single mode for lower power consumption
#define LSM303_SETTINGS2 0x01 // Mag data-ready interrupt enable


/* USER CODE END PV */
#define LSM303_ACC_Z_L_A_MULTI_READ (LSM303_ACC_Z_L_A | 0x80)
#define LSM303_ACC_X_L_A_MULTI_READ (LSM303_ACC_X_L_A | 0x80)


// CTRL_REG1_A = [ODR3][ODR2][ODR1][ODR0][LPEN][ZEN][YEN][XEN]
#define LSM303_ACC_Z_ENABLE 0x04 // 0000 0100
#define LSM303_ACC_XYZ_ENABLE 0x07 // 0000 0111
#define LSM303_ACC_100HZ 0x50 //0101 0000
#define LSM303_ACC_1HZ 0x10 //0001 0000
#define LSM303_ACC_WHO_AM_I 0x0F //0001 0000

// CTRL_REG3_A = [CLICK][AOI1][AOI2][DRDY_1][DRDY_2][WTM][OVERRUN][---]
#define LSM303_ACC_I1_DRDY1 0x10 //0001 0000
#define LSM303_ACC_I1_DRDY2 0x08 //0000 1000

#define LSM303_ACC_RESOLUTION 2.0 //

#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif


#endif
