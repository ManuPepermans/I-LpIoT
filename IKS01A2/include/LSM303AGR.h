#include "stm32F4xx_hal.h"

#define LSM303AGR_A_I2C_ADRESS 0x32

static I2C_HandleTypeDef *hi2cSensor;
void setI2CInterface_LSM303AGR(I2C_HandleTypeDef *hi2c);
void LSM303AGR_A_read_reg(uint8_t Reg ,uint8_t Data[], uint8_t length);
void LSM303AGR_A_write_reg(uint8_t Reg ,uint8_t Data[]);

