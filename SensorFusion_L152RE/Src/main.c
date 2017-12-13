/**
 ******************************************************************************
 * File Name          : main.c
 * Description        : Main program body
 ******************************************************************************
 ** This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * COPYRIGHT(c) 2017 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l1xx_hal.h"

/* USER CODE BEGIN Includes */
#include "LPS22HB_driver.h"
#include <string.h>
#include <stdio.h>
#include <strings.h>
#include <limits.h>
#include <math.h>

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define LSM303_ACC_ADDRESS 0x32 // 0011001 << 1 ADRESS from the accelero
#define LSM303_MAG_ADDRESS 0x3C //0011110 << 1 Address magnetometer

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

#define LSM303_ACC_Z_L_A_MULTI_READ (LSM303_ACC_Z_L_A | 0x80)
#define LSM303_ACC_X_L_A_MULTI_READ (LSM303_ACC_X_L_A | 0x80)

#define LSM303_ACC_RESOLUTION 2.0 //

typedef enum {
	TWOS_COMPLEMENT_24_BIT = 24, TWOS_COMPLEMENT_16_BIT = 16,
} SignBitIndex;

static uint8_t ender[] = { 0x0D, 0x0A };
static uint8_t rawSensorData[6];
static int64_t pressureRaw = 0;
static int16_t pressure = 0;
static int64_t temperatureRaw = 0;
static int16_t temperature = 0;
static uint8_t controlRegisterSettingLPS22HB[] = { 0x10 };
static uint8_t controlRegisterSettingsLSM303AGR_M[] = { 0x00 };
static uint8_t controlRegisterSettingsLSM303AGR_A[] = { 0x57 };
static char msg[128] = { [0 ... 127] = '\0' };

uint8_t Data[6]; // Var for acc data
uint8_t OUTX_L_REG_M_Data;
uint8_t OUTX_H_REG_M_Data;
uint8_t OUTY_L_REG_M_Data;
uint8_t OUTY_H_REG_M_Data;
uint8_t OUTZ_L_REG_M_Data;
uint8_t OUTZ_H_REG_M_Data;

uint16_t MAG_X;
uint16_t MAG_Y;
uint16_t MAG_Z;

int16_t Xaxis = 0; //
int16_t Yaxis = 0; //
int16_t Zaxis = 0; //

float Xaxis_g = 0; //
float Yaxis_g = 0; //
float Zaxis_g = 0; //

char str[50];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
 set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
int64_t twosComplementToSignedInteger(uint32_t rawValue, SignBitIndex sbi);

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void) {

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_I2C1_Init();

	/* USER CODE BEGIN 2 */
	//Configure the LPS22HB sensor
	HAL_I2C_Mem_Write(&hi2c1, 0xBA, 0x10, I2C_MEMADD_SIZE_8BIT,
			controlRegisterSettingLPS22HB, 1, HAL_MAX_DELAY);
	while (HAL_I2C_IsDeviceReady(&hi2c1, 0xBA, 1, HAL_MAX_DELAY) != HAL_OK)
		;
	//Configure the LSM303AGR accelerometer (see startup datasheet)
	HAL_I2C_Mem_Write(&hi2c1, 0x32, 0x20, I2C_MEMADD_SIZE_8BIT, 0x57, 1,
	HAL_MAX_DELAY);
	while (HAL_I2C_IsDeviceReady(&hi2c1, 0x32, 1, HAL_MAX_DELAY) != HAL_OK)
		;
	//Configure the LSM303AGR magnetometer (see startup datasheet)
	HAL_I2C_Mem_Write(&hi2c1, 0x3C, 0x60, I2C_MEMADD_SIZE_8BIT, 0x00, 1,
	HAL_MAX_DELAY);
	while (HAL_I2C_IsDeviceReady(&hi2c1, 0x3C, 1, HAL_MAX_DELAY) != HAL_OK)
		;

	HAL_Delay(500);

	printf(
			"Waarden naar de registers geschreven, apparaten klaar voor gebruik!\n\r");
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		printf("Start met barometer\r\n");
		HAL_I2C_Mem_Read(&hi2c1, 0xBA, 0x28, I2C_MEMADD_SIZE_8BIT,
				rawSensorData, 5, HAL_MAX_DELAY);

		uint32_t pressureTwosComplement = (rawSensorData[2] << 16)
				| (rawSensorData[1] << 8) | rawSensorData[0];
		pressureRaw = twosComplementToSignedInteger(pressureTwosComplement,
				TWOS_COMPLEMENT_24_BIT);
		pressure = (int16_t) (pressureRaw / 410);
		sprintf(msg, "%d µbar", pressure);
		HAL_UART_Transmit(&huart2, (uint8_t *) msg, 21, HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, ender, 2, HAL_MAX_DELAY);

		uint32_t temperatureTwosComplement = (rawSensorData[4] << 8)
				| rawSensorData[3];
		temperatureRaw = twosComplementToSignedInteger(
				temperatureTwosComplement, TWOS_COMPLEMENT_16_BIT);
		temperature = (int16_t) (temperatureRaw / 100);
		for (uint16_t i = 0; i < (sizeof(msg) / sizeof(msg[0])); ++i)
			msg[i] = '\0';
		sprintf(msg, "%d °C", temperature);
		HAL_UART_Transmit(&huart2, (uint8_t *) msg, 21, HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, ender, 2, HAL_MAX_DELAY);

		HAL_Delay(1000);

		printf("Start met ecompass\r\n");

		HAL_I2C_Mem_Read(&hi2c1, LSM303_ACC_ADDRESS,
		LSM303_ACC_X_L_A_MULTI_READ, 1, Data, 6, 100);
		HAL_UART_Transmit(&huart2, Data, 6, HAL_MAX_DELAY);

		HAL_Delay(500);

		//Read magnetometer dataregister(1) and print
		HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, OUTX_L_REG_M, 1,
				&OUTX_L_REG_M_Data, 1, 100);
		printf("Dataregister 1: %d\r\n", OUTX_L_REG_M_Data);

		//Read magnetometer dataregister(2) and print
		HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, OUTX_H_REG_M, 1,
				&OUTX_H_REG_M_Data, 1, 100);
		printf("Dataregister 2: %d\r\n", OUTX_H_REG_M_Data);

		//Read magnetometer dataregister(3) and print
		HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, OUTY_L_REG_M, 1,
				&OUTY_L_REG_M_Data, 1, 100);
		printf("Dataregister 3: %d\r\n", OUTY_L_REG_M_Data);

		//Read magnetometer dataregister(4) and print
		HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, OUTY_H_REG_M, 1,
				&OUTY_H_REG_M_Data, 1, 100);
		printf("Dataregister 4: %d\r\n", OUTY_H_REG_M_Data);

		//Read magnetometer dataregister(5) and print
		HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, OUTZ_L_REG_M, 1,
				&OUTZ_L_REG_M_Data, 1, 100);
		printf("Dataregister 5: %d\r\n", OUTZ_L_REG_M_Data);

		//Read magnetometer dataregister(6) and print
		HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, OUTZ_H_REG_M, 1,
				&OUTZ_H_REG_M_Data, 1, 100);
		printf("Dataregister 6: %d\r\n", OUTZ_H_REG_M_Data);

		//Omzetten van hexadecimaal naar decimal two's complement

		MAG_X = (OUTX_H_REG_M_Data << 8) | OUTX_L_REG_M_Data;
		MAG_Y = (OUTY_H_REG_M_Data << 8) | OUTY_L_REG_M_Data;
		MAG_Z = (OUTZ_H_REG_M_Data << 8) | OUTZ_L_REG_M_Data;

		int64_t MAG_X_int = twosComplementToSignedInteger(MAG_X, TWOS_COMPLEMENT_16_BIT);
		int64_t MAG_Y_int = twosComplementToSignedInteger(MAG_Y, TWOS_COMPLEMENT_16_BIT);
		int64_t MAG_Z_int = twosComplementToSignedInteger(MAG_Z, TWOS_COMPLEMENT_16_BIT);

		MAG_X_int = MAG_X_int*1.5;
		MAG_Y_int = MAG_Y_int*1.5;
		MAG_Z_int = MAG_Z_int*1.5;

		printf("MAG_X_int is: %d\r\n", MAG_X_int);
		printf("MAG_Y_int is: %d\r\n", MAG_Y_int);
		printf("MAG_Z_int is: %d\r\n", MAG_Z_int);

//		int mask = 0xFF; // 11111111
//
//		MAG_X ^= mask;
//		MAG_Y ^= mask;
//		MAG_Z ^= mask;
//
//		MAG_X = (int) MAG_X + 1;
//		MAG_Y = (int) MAG_Y + 1;
//		MAG_Z = (int) MAG_Z + 1;
//
//		printf("MAG_X %d\r\n", MAG_X);
//		printf("MAG_Y %d\r\n", MAG_Y);
//		printf("MAG_Z %d\r\n", MAG_Z);
//
//		Xaxis = ((Data[1] << 8) | Data[0]);
//		Yaxis = ((Data[3] << 8) | Data[2]);
//		Zaxis = ((Data[5] << 8) | Data[4]);
//
//		Xaxis_g = ((float) Xaxis * LSM303_ACC_RESOLUTION) / (float) INT16_MAX;
//		Yaxis_g = ((float) Yaxis * LSM303_ACC_RESOLUTION) / (float) INT16_MAX;
//		Zaxis_g = ((float) Zaxis * LSM303_ACC_RESOLUTION) / (float) INT16_MAX;
//		printf("Xaxis_g %d\r\n", Xaxis_g);
//		printf("Yaxis_g %d\r\n", Yaxis_g);
//		printf("Zaxis_g %d\r\n", Zaxis_g);

		HAL_Delay(3000);

	}
	/* USER CODE END 3 */

}

PUTCHAR_PROTOTYPE {
	/* Place your implementation of fputc here */
	/* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
	HAL_UART_Transmit(&huart2, (uint8_t *) &ch, 1, 0xFFFF);

	return ch;
}

/** System Clock Configuration
 */
void SystemClock_Config(void) {

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	/**Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = 16;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
	RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the Systick interrupt time
	 */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* I2C1 init function */
static void MX_I2C1_Init(void) {

	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* USART2 init function */
static void MX_USART2_UART_Init(void) {

	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/** Configure pins as 
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 */
static void MX_GPIO_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE()
	;
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : LD2_Pin */
	GPIO_InitStruct.Pin = LD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
int64_t twosComplementToSignedInteger(uint32_t rawValue, SignBitIndex sbi) {
	switch (sbi) {
	case TWOS_COMPLEMENT_24_BIT: {
		if ((rawValue & 0x00800000) == 0)
			return (rawValue & 0x00FFFFFF);
		return ((int64_t) (~((rawValue & 0x00FFFFFF) - 1))) * -1;
	}
	case TWOS_COMPLEMENT_16_BIT: {
		if ((rawValue & 0x00008000) == 0)
			return (rawValue & 0x0000FFFF);
		return ((int64_t) (~((rawValue & 0x0000FFFF) - 1))) * -1;
	}
	default:
		return NAN;
	}
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void _Error_Handler(char * file, int line) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */

}

#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
