/**
 ******************************************************************************
 * @name         main.hpp
 * @brief        This file contains the common defines of the application
 * @authors Imad Bouhadan, Dellafaille Lander, Marien Levi, Pepermans Manu, Van de Mieroop Kevin
 ******************************************************************************
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes -------------------------------------*/
#include "stm32l1xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <limits.h>

/* Defines for the main body -------------------------------------*/
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define LENGTH_ARRAY(x)  (sizeof(x) / sizeof(uint8_t))
#define SERIAL 7
#define BUFSIZE 200
#define CMD_LENGTH 19
#define FRAME_LENGTH

/* Function prototypes -------------------------------------*/
void sendGPS(void);
void initLora();
void initGPS(void);
void DASH7Message(uint8_t[], int);
void loraError(void);
void checkNetwork(void);

/* Variables used in the main body -------------------------------------*/
typedef int bool;
enum {
false, true
};

enum states {
in_danger_zone,
lora_ready,
no_lora,
safe_zone,
alarm_state,
lora_sending,
dash7_downlink
};
enum states state;

//global variables
//bool dangerZone;
//bool loraJoined;
int loraTries;
bool lora_gps_powered;

/* LPS22HB -------------------------------------*/
#define LPS22HB_BARO_ADDRESS 0xBA // ADRESS from the barometer  "10111010"
#define LPS22HB_AWHO_AM_I 0x0F
#define LPS22HB_CTRL_REG2 0x11
#define LPS22HB_PRESS_OUT_XL 0x28	// Pressure output value (LSB)
#define LPS22HB_OUT_L 0x29			// Pressure output value (mid part)
#define LPS22HB_OUT_H 0x2A			// Pressure output value (MSB)
#define LPS22HB_TEMP_OUT_L 0x2B		// Temperature output value (LSB)
#define LPS22HB_TEMP_OUT_H 0x2C		// Temperature output value (MSB)
#define LPS22HB_ONE__SHOT_ENABLE 0x11 // 0001 0001

uint8_t Data[6];
uint8_t LSBpressure;
uint8_t MIDpressure;
uint8_t MSBpressure;
uint8_t MSBtemp;
uint8_t LSBtemp;
uint32_t CompletePressure;
uint16_t temperature;
uint8_t Settings;

void getAndSendAltitude();

/* LSM303AGR -------------------------------------*/
#define PI 3.14159265359
#define resolution 8

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

/*LSM303 WHO_AM_I response*/
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

float lsm303agrAccDataTemp;
float rollRad;
float rollDegree;
float pitchRad;
float pitchDegree;
int32_t lsm303agrAccData[3];
float lsm303agrAccDataFloat[3];
int32_t lsm303agrMagData[3];
float lsm303agrMagDataFloat[3];
float yawTemp[3];
float yawRad;
float yawDegree;
uint16_t dir;

void LSM303AGR_readRegister(uint8_t LSM303AGR_reg, uint8_t LSM303AGR_data,
	uint8_t ACC_MAG);
void LSM303AGR_writeRegister(uint8_t LSM303AGR_reg, uint8_t LSM303AGR_data,
	uint8_t ACC_MAG);
void LSM303AGR_init();
void LSM303AGR_ACC_readAccelerationData(int32_t *pData);
void LSM303AGR_MAG_readMagneticData(int32_t *pData);
void LSM303AGR_MAG_readMagneticRawData(int16_t *pData);
void ecompassAlgorithm();

#ifdef __cplusplus
extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

/**
 * @}
 */

/**
 * @}
 */

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
