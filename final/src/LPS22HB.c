/* Includes ------------------------------------------------------------------*/
#include "LPS22HB.h"
//#include "stm32l1xx_hal.h"

static I2C_HandleTypeDef *hi2c_baro;
/* Private function prototypes -----------------------------------------------*/
void setBarInterface(I2C_HandleTypeDef *hi2c1);


void setBarInterface(I2C_HandleTypeDef *hi2c1)
{
	hi2c_baro = hi2c1;
	}

void initBar(void)
{
	uint8_t Settings = LPS22HB_ONE__SHOT_ENABLE;
	HAL_I2C_Mem_Write(hi2c_baro, LPS22HB_BARO_ADDRESS, LPS22HB_CTRL_REG2, 1, &Settings, 1, 100);
	}

void readBarometer(LPS22HB_TypeDef *baro)
{		//Read barometer dataRegister(LSB) and print
HAL_I2C_Mem_Read(&hi2c_baro, LPS22HB_BARO_ADDRESS, LPS22HB_PRESS_OUT_XL, 1, baro -> LSBpressure, 1, 100);

	//Read barometer dataRegister(MID) and print
	HAL_I2C_Mem_Read(&hi2c_baro, LPS22HB_BARO_ADDRESS, LPS22HB_OUT_L, 1, baro -> MIDpressure, 1, 100);
	//HAL_UART_Transmit(&huart2,&MIDpressure,1,HAL_MAX_DELAY);

	//Read barometer dataRegister(MSB) and print
	HAL_I2C_Mem_Read(&hi2c_baro, LPS22HB_BARO_ADDRESS, LPS22HB_OUT_H, 1, baro -> MSBpressure, 1, 100);
	//HAL_UART_Transmit(&huart2,&MSBpressure,1,HAL_MAX_DELAY);

	}
