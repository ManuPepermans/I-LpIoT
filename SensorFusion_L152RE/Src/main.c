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
//#include "LPS22HB_driver.cpp"
//#include "LSM303AGR_driver.cpp"
#include <string.h>
#include <stdio.h>
#include <strings.h>
#include <limits.h>
#include <math.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim3;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
typedef short int i16_t;
typedef unsigned char u8_t;

typedef enum {
	TWOS_COMPLEMENT_24_BIT = 24, TWOS_COMPLEMENT_16_BIT = 16,
} SignBitIndex;

typedef union {
	i16_t i16bit[3];
	u8_t u8bit[6];
} LSM303AGR_ACC_DATA;

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

HAL_I2C_StateTypeDef status = HAL_OK;
//I2C address of the accelerometer and magnetometer
//Shift already applied
#define LSM303AGR_ACC_I2C_ADDRESS         0x32
#define LSM303AGR_MAG_I2C_ADDRESS         0x3C

//Who am I
#define LSM303AGR_ACC_WHO_AM_I         0x33
#define LSM303AGR_MAG_WHO_AM_I         0x40

//Important register accelerometer
#define LSM303AGR_ACC_WHO_AM_I_REG      0X0F
#define LSM303AGR_ACC_TEMP_CFG_REG      0X1F
#define LSM303AGR_ACC_CTRL_REG1     0X20
#define LSM303AGR_ACC_CTRL_REG2     0X21
#define LSM303AGR_ACC_CTRL_REG3     0X22
#define LSM303AGR_ACC_CTRL_REG4     0X23
#define LSM303AGR_ACC_CTRL_REG5     0X24
#define LSM303AGR_ACC_CTRL_REG6     0X25
#define LSM303AGR_ACC_OUT_X_L   0X28
#define LSM303AGR_ACC_OUT_X_H   0X29
#define LSM303AGR_ACC_OUT_Y_L   0X2A
#define LSM303AGR_ACC_OUT_Y_H   0X2B
#define LSM303AGR_ACC_OUT_Z_L   0X2C
#define LSM303AGR_ACC_OUT_Z_H   0X2D
#define LSM303AGR_ACC_MULTIREAD (LSM303AGR_ACC_OUT_X_L | 0x80)
#define LSM303_ACC_XYZ_ENABLE 0x07
#define LSM303_ACC_100HZ 0x50
#define LSM303_ACC_I1_DRDY2 0x08

#define LSM303AGR_MAG_WHO_AM_I_REG      0X4F
#define LSM303AGR_MAG_CFG_REG_A     0X60
#define LSM303AGR_MAG_CFG_REG_B     0X61
#define LSM303AGR_MAG_CFG_REG_C     0X62
#define LSM303AGR_MAG_OUTX_L_REG    0X68
#define LSM303AGR_MAG_OUTX_H_REG    0X69
#define LSM303AGR_MAG_OUTY_L_REG    0X6A
#define LSM303AGR_MAG_OUTY_H_REG    0X6B
#define LSM303AGR_MAG_OUTZ_L_REG    0X6C
#define LSM303AGR_MAG_OUTZ_H_REG    0X6D
#define LSM303AGR_MAG_MULTIREAD (LSM303AGR_MAG_OUTX_L_REG | 0x80)
#define LSM303_SETTINGS1 0x00 // Mag = 10 Hz (HR and continuous mode) //This has to be set to single mode for lower power consumption
#define LSM303_SETTINGS2 0x01 // Mag data-ready interrupt enable

#define X 0
#define Y 1
#define Z 2

int32_t axes_a[3];
int32_t axes_m[3];
float roll = 0, pitch = 0, roll_rad = 0, pitch_rad = 0;
float yaw_rad = 0, degree = 0, godr = 0;
float temp[3] = { }, hardCor[3] = { };

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM3_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
 set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */
PUTCHAR_PROTOTYPE {
	/* Place your implementation of fputc here */
	/* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
	HAL_UART_Transmit(&huart2, (uint8_t *) &ch, 1, 0xFFFF);

	return ch;
}
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
	MX_TIM3_Init();

	/* USER CODE BEGIN 2 */
	//Check the who am I
	// 5ms boot procedure
	HAL_Delay(10);

	// Reading the who am I register for checking
	uint8_t hwID[1];
	HAL_I2C_Mem_Read(&hi2c1, LSM303AGR_ACC_I2C_ADDRESS,
	LSM303AGR_ACC_WHO_AM_I_REG, I2C_MEMADD_SIZE_8BIT, hwID, sizeof(hwID),
	HAL_MAX_DELAY);
	if (hwID[0] == LSM303AGR_ACC_WHO_AM_I) {
		printf("WHO_AM_I of accelerometer correct!!\r\n");
	} else {
		printf("Error!!\r\n");
	}

	uint8_t Settings = LSM303_ACC_XYZ_ENABLE | LSM303_ACC_100HZ;
	// I2C write settings
	HAL_I2C_Mem_Write(&hi2c1, LSM303AGR_ACC_I2C_ADDRESS,
	LSM303AGR_ACC_CTRL_REG1, 1, &Settings, 1, 100);
	Settings = LSM303_ACC_I1_DRDY2;
	HAL_I2C_Mem_Write(&hi2c1, LSM303AGR_ACC_I2C_ADDRESS,
	LSM303AGR_ACC_CTRL_REG3, 1, &Settings, 1, 100);

	// Mag = 10 Hz (HR and continuous mode) //This has to be set to single mode for lower power consumption
	Settings = LSM303_SETTINGS1;
	HAL_I2C_Mem_Write(&hi2c1, LSM303AGR_MAG_I2C_ADDRESS,
	LSM303AGR_MAG_CFG_REG_A, 1, &Settings, 1, 100);

	// Mag data-ready interrupt enable
	Settings = LSM303_SETTINGS2;
	HAL_I2C_Mem_Write(&hi2c1, LSM303AGR_MAG_I2C_ADDRESS,
	LSM303AGR_MAG_CFG_REG_C, 1, &Settings, 1, 100);

	printf("Devices ready\r\n");
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		get_x_axes(axes_a);
		printf("LSM303AGR [acc/mg]:  %6ld, %6ld, %6ld\r\n", axes_a[0],
				axes_a[1], axes_a[2]);

		get_m_axes(axes_m);
		printf("LSM303AGR [mag/mgauss]:  %6ld, %6ld, %6ld\r\n", axes_m[0],
				axes_m[1], axes_m[2]);

		HAL_Delay(1000);

		printf("\r\n");
		roll_rad = atan2((float) axes_a[Y], axes_a[Z]);
		roll = roll_rad * 57.296;

		temp[0] = (float) axes_a[Y] * sin(roll_rad);
		temp[1] = cos(roll_rad) * (float) axes_a[Z] + temp[0];
		pitch_rad = -atan2((float) axes_a[X],
				sqrt(pow((float) axes_a[Y], 2) + pow((float) axes_a[Z], 2)));//temp[1]);
		pitch = pitch_rad * 57.296;
		printf("Angle: roll: %6lf   pitch: %6lf \r\n ", roll, pitch);

		//Determining where the head of the compass is pointed at
		temp[0] = axes_m[Z] * sin(roll_rad) - axes_m[Y] * cos(roll_rad);
		temp[1] = axes_m[Y] * sin(roll_rad) + axes_m[Z] * cos(roll_rad);
		temp[2] = axes_m[X] * cos(pitch_rad) + temp[1] * sin(pitch_rad);
		yaw_rad = atan2((float) temp[0], temp[2]);
		degree = yaw_rad * 57.296; //180/PI
		degree = degree + 50;

		//Transforming degrees into a wind direction
		if (degree > 360) {
			degree = degree - 360;
		}
		if (degree < 0) {
			degree = degree + 360;
		}
		printf("degree: %lf\r\n", degree);
		if (degree > 22.5 && degree < 67.5) {
			printf("Direction: North-East\r\n");
		} else if (degree > 67.5 && degree < 112.5) {
			printf("Direction: East\r\n");
		} else if (degree > 112.5 && degree < 157.5) {
			printf("Direction: South-East\r\n");
		} else if (degree > 157.5 && degree < 202.5) {
			printf("Direction: South\r\n");
		} else if (degree > 202.5 && degree < 247.5) {
			printf("Direction: South-West\r\n");
		} else if (degree > 247.5 && degree < 292.5) {
			printf("Direction: West\r\n");
		} else if (degree > 292.5 && degree < 337.25) {
			printf("Direction: North-West\r\n");
		} else if (degree > 337.25 || degree < 22.5) {
			printf("Direction: North\r\n");
		}

		HAL_Delay(1000);

	}
	/* USER CODE END 3 */

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

/* TIM3 init function */
static void MX_TIM3_Init(void) {

	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;

	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 32000;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 3000;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig)
			!= HAL_OK) {
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

HAL_I2C_StateTypeDef LSM303AGR_ACC_readRegister(uint8_t reg, uint8_t pData[]) {

	return HAL_I2C_Mem_Read(&hi2c1, LSM303AGR_ACC_I2C_ADDRESS, reg,
	I2C_MEMADD_SIZE_8BIT, pData, sizeof(pData), HAL_MAX_DELAY);
}

HAL_I2C_StateTypeDef LSM303AGR_MAG_readRegister(uint8_t reg, uint8_t pData[]) {

	return HAL_I2C_Mem_Read(&hi2c1, LSM303AGR_MAG_I2C_ADDRESS, reg,
	I2C_MEMADD_SIZE_8BIT, pData, sizeof(pData), HAL_MAX_DELAY);

}

/**
 * @brief  Read data from LSM303AGR Accelerometer
 * @param  pData the pointer where the accelerometer data are stored
 * @retval 0 in case of success, an error code otherwise
 */
void get_x_axes(int32_t *pData) {

	int data[3];

	/* Read data from LSM303AGR. */
	LSM303AGR_ACC_Get_Acceleration(data);

	/* Calculate the data. */
	pData[0] = (int32_t) data[0];
	pData[1] = (int32_t) data[1];
	pData[2] = (int32_t) data[2];

}

/*
 * Following is the table of sensitivity values for each case.
 * Values are espressed in ug/digit.
 */
void LSM303AGR_ACC_Get_Acceleration(int *buff) {

	LSM303AGR_ACC_DATA raw_data_tmp;

	// Normal mode 10-bit, shift = 6 and FS = 2
	unsigned char op_mode = 2, fs_mode = 0, shift = 4;

	/* Read out raw accelerometer samples */
	//LSM303AGR_ACC_readRegister(LSM303AGR_ACC_MULTIREAD, raw_data_tmp.u8bit);
	HAL_I2C_Mem_Read(&hi2c1, LSM303AGR_ACC_I2C_ADDRESS, LSM303AGR_ACC_MULTIREAD,
	I2C_MEMADD_SIZE_8BIT, raw_data_tmp.u8bit, sizeof(raw_data_tmp.u8bit),
	HAL_MAX_DELAY);

	raw_data_tmp.i16bit[0] = (raw_data_tmp.u8bit[1] << 8)
			| raw_data_tmp.u8bit[0];
	raw_data_tmp.i16bit[1] = (raw_data_tmp.u8bit[3] << 8)
			| raw_data_tmp.u8bit[2];
	raw_data_tmp.i16bit[2] = (raw_data_tmp.u8bit[5] << 8)
			| raw_data_tmp.u8bit[4];

	/* Apply proper shift and sensitivity */
	buff[0] = ((raw_data_tmp.i16bit[0] >> shift)
			* LSM303AGR_ACC_Sensitivity_List[op_mode][fs_mode] + 500) / 1000;
	buff[1] = ((raw_data_tmp.i16bit[1] >> shift)
			* LSM303AGR_ACC_Sensitivity_List[op_mode][fs_mode] + 500) / 1000;
	buff[2] = ((raw_data_tmp.i16bit[2] >> shift)
			* LSM303AGR_ACC_Sensitivity_List[op_mode][fs_mode] + 500) / 1000;

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
void get_m_axes_raw(int16_t *pData) {

	uint8_t regValue[6] = { 0, 0, 0, 0, 0, 0 };
	int16_t *regValueInt16;
	//Type3Axis16bit_U raw_data_tmp;

	/* Read output registers from LSM303AGR_MAG_OUTX_L to LSM303AGR_MAG_OUTZ_H. */
//	LSM303AGR_MAG_readRegister(LSM303AGR_ACC_MULTIREAD, regValue);
	HAL_I2C_Mem_Read(&hi2c1, LSM303AGR_MAG_I2C_ADDRESS, LSM303AGR_MAG_MULTIREAD,
	I2C_MEMADD_SIZE_8BIT, regValue, sizeof(regValue), HAL_MAX_DELAY);

	regValueInt16 = (int16_t *) regValue;

	/* Format the data. */
	pData[0] = regValueInt16[0];
	pData[1] = regValueInt16[1];
	pData[2] = regValueInt16[2];

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
