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
#define LSM303_ACC_ADDRESS (0x19 << 1) // ADRESS from the accelero
#define LSM303_MAG_ADDRESS 0x3C // ADRESS from the magnetometer

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

#define LSM303_MAG_RSP 0x40
#define LSM303_MAG_WHO_AM_I 0x4F
#define LSM303_ACC_RSP 0x33

#define LSM303_ACC_RESOLUTION 2.0 //

typedef enum {
	TWOS_COMPLEMENT_24_BIT = 24, TWOS_COMPLEMENT_16_BIT = 16,
} SignBitIndex;

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
	uint8_t Settings = LSM303_ACC_XYZ_ENABLE | LSM303_ACC_100HZ;

	// I2C write settings
	//Startup sequence LSM303AGR
	HAL_I2C_Mem_Write(&hi2c1, LSM303_MAG_ADDRESS, LSM303_CFG_REG_A_M,
	I2C_MEMADD_SIZE_8BIT, 0x00, sizeof(0x00), 100);
	while (HAL_I2C_IsDeviceReady(&hi2c1, LSM303_MAG_ADDRESS, 1, HAL_MAX_DELAY)
			!= HAL_OK)
		;
	HAL_I2C_Mem_Write(&hi2c1, LSM303_MAG_ADDRESS, LSM303_CFG_REG_C_M,
	I2C_MEMADD_SIZE_8BIT, 0x01, sizeof(0x01), 100);
	while (HAL_I2C_IsDeviceReady(&hi2c1, LSM303_MAG_ADDRESS, 1, HAL_MAX_DELAY)
			!= HAL_OK)
		;

	uint8_t hwID[1];
	HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, LSM303_MAG_WHO_AM_I,
	I2C_MEMADD_SIZE_8BIT, hwID, 1, HAL_MAX_DELAY);
	if (hwID[0] == LSM303_MAG_RSP) {
		printf("WHO_AM_I of magnetometer correct!!\r\n");
	} else {
		printf("Error!!\r\n");
	}

	// I2C write settings
	HAL_I2C_Mem_Write(&hi2c1, LSM303_ACC_ADDRESS, LSM303_ACC_CTRL_REG1_A, 1,
			&Settings, 1, 100);
	Settings = LSM303_ACC_I1_DRDY2;
	HAL_I2C_Mem_Write(&hi2c1, LSM303_ACC_ADDRESS, LSM303_ACC_CTRL_REG3_A, 1,
			&Settings, 1, 100);

	HAL_I2C_Mem_Read(&hi2c1, LSM303_ACC_ADDRESS, LSM303_ACC_WHO_AM_I,
	I2C_MEMADD_SIZE_8BIT, hwID, 1, HAL_MAX_DELAY);
	if (hwID[0] == LSM303_ACC_RSP) {
		printf("WHO_AM_I of accelerometer correct!!\r\n");
	} else {
		printf("Error!!\r\n");
	}

	printf("Devices ready\r\n");
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {

		int32_t axes_m[3];
		int32_t axes_a[3];
		float axes_aF[3];
		float axes_aT[3];
		float roll, pitch, x, y, h, dir;
		uint16_t dirUint16_t;

		get_m_axes(axes_m);

		//printf("LSM303AGR [mag/mgauss]:  %6ld, %6ld, %6ld\r\n", axes_m[0],
		//axes_m[1], axes_m[2]);
		//LSM303AGR_ACC_Get_Acceleration(axes_a);

		//printf("LSM303AGR [acc/mg]:  %6ld, %6ld, %6ld\r\n", axes_a[0],
		//	axes_a[1], axes_a[2]);

		HAL_I2C_Mem_Read(&hi2c1, LSM303_ACC_ADDRESS,
				LSM303_ACC_X_L_A_MULTI_READ, 1, Data, 6, 100);
		HAL_Delay(500);

		Xaxis = ((Data[1] << 8) | Data[0]);
		Yaxis = ((Data[3] << 8) | Data[2]);
		Zaxis = ((Data[5] << 8) | Data[4]);

		axes_a[0] = ((Xaxis >> 6) * 3900 + 500) / 1000;
		axes_a[1] = ((Yaxis >> 6) * 3900 + 500) / 1000;
		axes_a[2] = ((Zaxis >> 6) * 3900 + 500) / 1000;

		printf("LSM303AGR [acc/mg]:  %6ld, %6ld, %6ld\r\n", Xaxis_g, Xaxis_g,
				Xaxis_g);

		axes_aF[0] = (float) axes_a[0];
		    axes_aF[1] = (float) axes_a[1];
		    axes_aF[2] = (float) axes_a[2];

		    // Resolution is 2^(8) = 256
		    axes_aT[0] = (axes_aF[0] * 2 / 256);
		    axes_aT[1] = (axes_aF[1] * 2 / 256);
		    axes_aT[2] = (axes_aF[2] * 2 / 256);

		    // Some math to get the thing working
		    roll = atan2f(axes_aT[1], axes_aT[2]);
		    pitch = atan2f(-axes_aT[0],
		            axes_aT[1] * sinf(roll) + axes_aT[2] * cosf(roll));

		    x = axes_m[0] * cosf(pitch)
		            + (axes_m[1] * sinf(roll) + axes_m[2] * cosf(roll)) * sinf(pitch);
		    y = axes_m[2] * sinf(roll) - axes_m[1] * cosf(roll);

		    // Convert to degrees, look in what quadrant the value lays and if it's negative add 360° to achieve correct positioning.
		    h = atan2f(y, x);
		    dir = h * (180 / 3.1415);
		    if (dir < 0)
		        dir += 360;

		    // Convert for transmission over DASH7
		    dirUint16_t = (uint16_t)dir;

		    printf("LSM303AGR [mag/mgauss]:  %6ld, %6ld, %6ld\r\n", axes_m[0],
		            axes_m[1], axes_m[2]);
		    printf("LSM303AGR [acc/mg]:  %6ld, %6ld, %6ld\r\n", axes_a[0], axes_a[1],
		            axes_a[2]);
		    printf("richting: %6ld graden\r\n", dirUint16_t);

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
	huart2.Init.BaudRate = 9600;
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

/**
 * @brief  Read raw data from LSM303AGR Magnetometer
 * @param  pData the pointer where the magnetomer raw data are stored
 * @retval 0 in case of success, an error code otherwise
 */
void get_m_axes(int32_t *pData) {
	int16_t pDataRaw[3];
	float sensitivity = 1.5;

	/* Read raw data from LSM303AGR output register. */
	get_m_axes_raw(pDataRaw);

	/* Calculate the data. */
	pData[0] = (int32_t) (pDataRaw[0] * sensitivity);
	pData[1] = (int32_t) (pDataRaw[1] * sensitivity);
	pData[2] = (int32_t) (pDataRaw[2] * sensitivity);
}

/**
 * @brief  Read raw data from LSM303AGR Magnetometer
 * @param  pData the pointer where the magnetomer raw data are stored
 * @retval 0 in case of success, an error code otherwise
 */
int get_m_axes_raw(int16_t *pData) {
	uint8_t regValue[6] = { 0, 0, 0, 0, 0, 0 };
	int16_t *regValueInt16;

	/* Read output registers from LSM303AGR_MAG_OUTX_L to LSM303AGR_MAG_OUTZ_H. */
	HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, OUTX_L_REG_M,
	I2C_MEMADD_SIZE_8BIT, &regValue[0], 1, 100);
	HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, OUTX_H_REG_M,
	I2C_MEMADD_SIZE_8BIT, &regValue[1], 1, 100);
	HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, OUTY_L_REG_M,
	I2C_MEMADD_SIZE_8BIT, &regValue[2], 1, 100);
	HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, OUTY_H_REG_M,
	I2C_MEMADD_SIZE_8BIT, &regValue[3], 1, 100);
	HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, OUTZ_L_REG_M,
	I2C_MEMADD_SIZE_8BIT, &regValue[4], 1, 100);
	HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, OUTZ_H_REG_M,
	I2C_MEMADD_SIZE_8BIT, &regValue[5], 1, 100);

	regValueInt16 = (int16_t *) regValue;

	/* Format the data. */
	pData[0] = regValueInt16[0];
	pData[1] = regValueInt16[1];
	pData[2] = regValueInt16[2];

}

///**
// * @brief  Read data from LSM303AGR Accelerometer
// * @param  pData the pointer where the accelerometer data are stored
// * @retval 0 in case of success, an error code otherwise
// */
//void get_x_axes(int32_t *pData) {
//	int data[3];
//
//	/* Calculate the data. */
//	pData[0] = (int32_t) data[0];
//	pData[1] = (int32_t) data[1];
//	pData[2] = (int32_t) data[2];
//}

/*
 * Values returned are espressed in mg.
 */
void LSM303AGR_ACC_Get_Acceleration(int32_t *pData) {
	int16_t pDataRaw[3];

	LSM303AGR_ACC_Get_Raw_Acceleration(pDataRaw);
//See driver, normal mode shift = 6

	/* Apply proper shift and sensitivity */
	pData[0] = ((pDataRaw[0] >> 6) * 3900 + 500) / 1000;
	pData[1] = ((pDataRaw[1] >> 6) * 3900 + 500) / 1000;
	pData[2] = ((pDataRaw[2] >> 6) * 3900 + 500) / 1000;
}

void LSM303AGR_ACC_Get_Raw_Acceleration(int16_t *pData) {
	uint8_t regValue[6] = { 0, 0, 0, 0, 0, 0 };
	int16_t *regValueInt16;

	HAL_I2C_Mem_Read(&hi2c1, LSM303_ACC_ADDRESS, LSM303_ACC_X_L_A_MULTI_READ, 1,
			Data, 6, 100);
	HAL_UART_Transmit(&huart2, Data, 6, HAL_MAX_DELAY);

//	HAL_I2C_Mem_Read(&hi2c1, LSM303_ACC_ADDRESS, LSM303_ACC_X_L_A,
//	I2C_MEMADD_SIZE_8BIT, &regValue[0], 1, 100);
//	HAL_I2C_Mem_Read(&hi2c1, LSM303_ACC_ADDRESS, LSM303_ACC_X_H_A,
//	I2C_MEMADD_SIZE_8BIT, &regValue[1], 1, 100);
//	HAL_I2C_Mem_Read(&hi2c1, LSM303_ACC_ADDRESS, LSM303_ACC_Y_L_A,
//	I2C_MEMADD_SIZE_8BIT, &regValue[2], 1, 100);
//	HAL_I2C_Mem_Read(&hi2c1, LSM303_ACC_ADDRESS, LSM303_ACC_Y_H_A,
//	I2C_MEMADD_SIZE_8BIT, &regValue[3], 1, 100);
//	HAL_I2C_Mem_Read(&hi2c1, LSM303_ACC_ADDRESS, LSM303_ACC_Z_L_A,
//	I2C_MEMADD_SIZE_8BIT, &regValue[4], 1, 100);
//	HAL_I2C_Mem_Read(&hi2c1, LSM303_ACC_ADDRESS, LSM303_ACC_Z_H_A,
//	I2C_MEMADD_SIZE_8BIT, &regValue[5], 1, 100);

	//HAL_UART_Transmit(&huart2, &regValue[3], 1, HAL_MAX_DELAY);
	//printf("Register 1: %d\r\n", regValue[0]);
	//printf("Register 2: %d\r\n", regValue[1]);
	//printf("Register 3: %d\r\n", regValue[2]);

//	regValueInt16 = (int16_t *) regValue;
//
//	/* Format the data. */
//	pData[0] = regValueInt16[0];
//	pData[1] = regValueInt16[1];
//	pData[2] = regValueInt16[2];

	Xaxis = ((Data[1] << 8) | Data[0]);
	Yaxis = ((Data[3] << 8) | Data[2]);
	Zaxis = ((Data[5] << 8) | Data[4]);
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
