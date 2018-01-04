/**
 ******************************************************************************
 * @file main.c
 * @author Mariën Levi
 * @date 04/01/2018
 * @brief Main program body of the ecompass.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l1xx_hal.h"

/* USER CODE BEGIN Includes */
#include "LSM303AGR.cpp"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim3;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_I2C1_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
 set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE {
	HAL_UART_Transmit(&huart2, (uint8_t *) &ch, 1, 0xFFFF);
	return ch;
}
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
	MX_TIM3_Init();
	MX_I2C1_Init();

	/* USER CODE BEGIN 2 */

	LSM303AGR_setI2CInterface(&hi2c1);
	LSM303AGR_ACC_reset();
	LSM303AGR_MAG_reset();
	HAL_Delay(10);
	LSM303AGR_init();

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

		dirInt = ecompassAlgorithm();
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

	/** SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/** I2C1 init function */
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

/** TIM3 init function */
static void MX_TIM3_Init(void) {

	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;

	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 31999;
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

/** USART2 init function */
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

/** Pinout Configuration
 */
static void MX_GPIO_Init(void) {

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;

}

/* USER CODE BEGIN 4 */
/**
 * The ecompass algorithm, it does the following:
 * 		1. Read the data from the accelerometer and magnetomerer
 * 		2. Convert the data from integer to float
 * 		3. Apply the resolution on the accelerometer data
 * 		4. Calculate the phi (roll angle) value
 * 		5. Calculate the  theta (pitch angle or attitude) value
 * 		6. Calculate of the Psi (yaw angle or heading) value
 *
 * @return the value of the heading in uint16_t format.
 */
uint16_t ecompassAlgorithm() {

	printf("Start ecompassAlgorithm\r\n");

//	LSM303AGR_wakeUpAccelerometer();
//	LSM303AGR_wakeUpMagnetometer();
//	HAL_Delay(100);
	LSM303AGR_ACC_readAccelerationData(lsm303agrAccData);
	LSM303AGR_MAG_readMagneticData(lsm303agrMagData);
//	LSM303AGR_powerDownAccelerometer();
//	LSM303AGR_powerDownMagnetometer();

	printf("Data accelerometer: %ld %ld %ld\r\n", lsm303agrAccData[0],
			lsm303agrAccData[1], lsm303agrAccData[2]);
	printf("Data Magnetometer: %ld %ld %ld\r\n", lsm303agrMagData[0],
			lsm303agrMagData[1], lsm303agrMagData[2]);

	printf("---------------------------------------------------------\r\n");

//Casting from integer to float (6 castings necessary)
	lsm303agrAccDataFloat[0] = lsm303agrAccData[0];
	lsm303agrAccDataFloat[1] = lsm303agrAccData[1];
	lsm303agrAccDataFloat[2] = lsm303agrAccData[2];

	lsm303agrMagDataFloat[0] = lsm303agrMagData[0];
	lsm303agrMagDataFloat[1] = lsm303agrMagData[1];
	lsm303agrMagDataFloat[2] = lsm303agrMagData[2];

	printf("Data accelerometer: %0.2f %0.2f %0.2f\r\n",
			lsm303agrAccDataFloat[0], lsm303agrAccDataFloat[1],
			lsm303agrAccDataFloat[2]);
	printf("Data Magnetometer: %0.2f %0.2f %0.2f\r\n", lsm303agrMagDataFloat[0],
			lsm303agrMagDataFloat[1], lsm303agrMagDataFloat[2]);

	printf("---------------------------------------------------------\r\n");

	lsm303agrAccDataFloat[0] = (lsm303agrAccDataFloat[0] * 2)
			/ pow(2, (resolution - 1));
	lsm303agrAccDataFloat[1] = (lsm303agrAccDataFloat[1] * 2)
			/ pow(2, (resolution - 1));
	lsm303agrAccDataFloat[2] = (lsm303agrAccDataFloat[2] * 2)
			/ pow(2, (resolution - 1));

	//Computation of Phi (roll angle) in radians and degrees
	rollRad = atan2f(lsm303agrAccDataFloat[1], lsm303agrAccDataFloat[2]);
	rollDegree = rollRad * (180 / PI);

	//Computation of Theta (pitch angle or attitude) in radians and degrees
	lsm303agrAccDataTemp = lsm303agrAccData[1] * sinf(rollRad)
			+ lsm303agrAccData[2] * cosf(rollRad);
	pitchRad = atan2f(-lsm303agrAccData[0], lsm303agrAccDataTemp);
	pitchDegree = pitchRad * (180 / PI);

	printf("Phi (roll angle, in degrees) is: %0.2f\r\n", rollDegree);
	printf("Theta (pitch angle or attitude, in degrees) is: %0.2f\r\n",
			pitchDegree);

	//Computation of Psi (yaw angle, or heading)
	yawTemp[0] = lsm303agrMagDataFloat[2] * sinf(rollRad)
			- lsm303agrMagDataFloat[1] * cosf(rollRad);
	yawTemp[1] = lsm303agrMagDataFloat[1] * sinf(rollRad)
			+ lsm303agrMagDataFloat[2] * cosf(rollRad);
	yawTemp[2] = lsm303agrMagDataFloat[0] * cosf(pitchRad)
			+ yawTemp[1] * sinf(pitchRad);
	yawRad = atan2f(yawTemp[0], yawTemp[2]);
	yawDegree = yawRad * (180 / PI);

//	x_direction = lsm303agrMagDataFloat[0] * cosf(pitchRad)
//			+ (lsm303agrMagDataFloat[1] * sinf(rollRad)
//					+ lsm303agrMagDataFloat[2] * cosf(rollRad))
//					* sinf(pitchRad);
//	y_direction = lsm303agrMagDataFloat[2] * sinf(rollRad)
//			- lsm303agrMagDataFloat[1] * cosf(rollRad);
//	yawRad = atan2f(y_direction, x_direction);
//	yawDegree = yawRad * (180 / PI);

	if (yawDegree < 0)
		yawDegree += 360;

	printf("Psi (yaw angle or heading, in degrees) is: %0.2f\r\n", yawDegree);

	printf("Finished ecompassAlgorithm\r\n");
	printf("---------------------------------------------------------\r\n");

	return (uint16_t) yawDegree;

}

/**
 * Callibrates the ecompass. The algorithm is based on: https://github.com/kriswiner/MPU-6050/wiki/Simple-and-Effective-Magnetometer-Calibration
 *
 */
void callibration() {

	int16_t axes[3];
	int16_t x_min, y_min, z_min;
	int16_t x_max, y_max, z_max;
	int16_t x_bias, y_bias, z_bias;
	uint8_t registerValue;

	printf("Start callibration\r\n");

	LSM303AGR_MAG_readMagneticRawData(axes);
	axes[0] = x_min = x_max;
	axes[1] = y_min = y_max;
	axes[2] = z_min = z_max;

	for (uint8_t i = 0; i < 127; ++i) {
		LSM303AGR_MAG_readMagneticRawData(axes);

		if (axes[0] > x_max)
			x_max = axes[0];
		else if (axes[0] < x_min)
			x_min = axes[0];

		if (axes[1] > y_max)
			y_max = axes[1];
		else if (axes[1] < y_min)
			y_min = axes[1];

		if (axes[2] > z_max)
			z_max = axes[2];
		else if (axes[2] < z_min)
			z_min = axes[2];

		HAL_Delay(50);
	}

	registerValue =



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
