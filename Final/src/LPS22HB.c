/*
 * LPS22HB.c
 *
 *  Created on: 21 Dec 2017
 *      Author: manupepermans
 */


/* Includes ------------------------------------------------------------------*/
#include "LPS22HB.h"
#include "stm32l1xx_hal.h"
#include "main.h"



/* Private function prototypes -----------------------------------------------*/

uint8_t LSBpressure1;
uint8_t MIDpressure1;
uint8_t MSBpressure1;


void setBarInterface(I2C_HandleTypeDef *hi2c1, UART_HandleTypeDef *uart)
{

	HAL_UART_Transmit(&uart_baro,"test",sizeof("test"),HAL_MAX_DELAY);

	hi2c_baro = hi2c1;
	uart_baro = uart;
}

void initBar(void)
{
	HAL_UART_Transmit(uart_baro,"test",sizeof("test"),HAL_MAX_DELAY);
	uint8_t Settings = LPS22HB_ONE__SHOT_ENABLE;
	HAL_I2C_Mem_Write(hi2c_baro, LPS22HB_BARO_ADDRESS, LPS22HB_CTRL_REG2, 1, &Settings, 1, 100);
	}


void readsomething(){
	HAL_I2C_Mem_Read(hi2c_baro, LPS22HB_BARO_ADDRESS, LPS22HB_PRESS_OUT_XL, 1, &LSBpressure1, 1, 100);
	HAL_UART_Transmit(uart_baro,&LSBpressure1,1,HAL_MAX_DELAY);
	HAL_Delay(1000);

	//return LSBpressure1[0];

}

uint8_t readBarometer(LPS22HB_TypeDef *baro)
{		//Read barometer dataRegister(LSB) and print
//	HAL_I2C_Mem_Read(&hi2c_baro, LPS22HB_BARO_ADDRESS, LPS22HB_PRESS_OUT_XL, 1, baro -> LSBpressure, 1, 100);
//	HAL_UART_Transmit(&huart2,baro -> LSBpressure,1,HAL_MAX_DELAY);
//
//	//Read barometer dataRegister(MID) and print
//	HAL_I2C_Mem_Read(&hi2c_baro, LPS22HB_BARO_ADDRESS, LPS22HB_OUT_L, 1, baro -> MIDpressure, 1, 100);
//	HAL_UART_Transmit(&huart2,&(baro -> MIDpressure),1,HAL_MAX_DELAY);
//
//	//Read barometer dataRegister(MSB) and print
//	HAL_I2C_Mem_Read(&hi2c_baro, LPS22HB_BARO_ADDRESS, LPS22HB_OUT_H, 1, baro -> MSBpressure, 1, 100);
//	HAL_UART_Transmit(&huart2,&(baro -> MSBpressure),1,HAL_MAX_DELAY);

	HAL_I2C_Mem_Read(&hi2c_baro, LPS22HB_BARO_ADDRESS, LPS22HB_PRESS_OUT_XL, 1, &LSBpressure1, 1, 100);
	//HAL_UART_Transmit(&huart2,&LSBpressure,1,HAL_MAX_DELAY);
	baro->LSBpressure = LSBpressure1;


	//Read barometer dataRegister(MID) and print
	HAL_I2C_Mem_Read(&hi2c_baro, LPS22HB_BARO_ADDRESS, LPS22HB_OUT_L, 1, &MIDpressure1, 1, 100);
	//HAL_UART_Transmit(&huart2,&MIDpressure,1,HAL_MAX_DELAY);
	baro -> MIDpressure = MIDpressure1;

	//Read barometer dataRegister(MSB) and print
	HAL_I2C_Mem_Read(&hi2c_baro, LPS22HB_BARO_ADDRESS, LPS22HB_OUT_H, 1, &MSBpressure1, 1, 100);
	//HAL_UART_Transmit(&huart2,&MSBpressure,1,HAL_MAX_DELAY);
	baro -> MSBpressure = MSBpressure1;
	return LSBpressure1;
}
