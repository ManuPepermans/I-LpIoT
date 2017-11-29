#include "LSM303AGR.h"

void setI2CInterface_LSM303AGR(I2C_HandleTypeDef *hi2c)
{
	hi2cSensor=hi2c;
}
void LSM303AGR_A_write_reg(uint8_t Reg ,uint8_t Data[])
{
	HAL_I2C_Master_Transmit(hi2cSensor, LSM303AGR_A_I2C_ADRESS, Data,sizeof(Data), HAL_MAX_DELAY);
}

void LSM303AGR_A_read_reg(uint8_t Reg ,uint8_t Data[], uint8_t length)
{
	HAL_I2C_Mem_Read(hi2cSensor, LSM303AGR_A_I2C_ADRESS, Reg, sizeof(uint8_t), Data, length, HAL_MAX_DELAY);
}
