
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart6;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART6_UART_Init(void);
static void sendGPS(void);
static void initLora(void);
static void initGPS(void);
typedef int bool;
enum {
	false, true
};
bool dangerZone;


int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();


  HAL_Delay(10000);

  initGPS();
  HAL_Delay(1000);
  initLora();
  HAL_Delay(1000);



  /* Infinite loop */
  while (1)

  {
	  HAL_Delay(2000);
	dangerZone = true;
	if (dangerZone) {
		HAL_UART_Transmit(&huart2,"Danger zone!\n", sizeof("Danger zone!\n")-1, HAL_MAX_DELAY);
		HAL_Delay(1000);
		sendGPS();
	}

  }
}


void initLora() {
	uint8_t buffer[64];
	uint8_t character[1];
	int i = 0;

	 uint8_t joinLora[] = {0x41,0x54,0x2b,0x4a,0x4f,0x49,0x4e, 0x0d, 0x0a};
	 HAL_UART_Transmit(&huart6, joinLora, 9, HAL_MAX_DELAY);
	 bool loraJoined = false;
	 while(loraJoined == false){

	 if (HAL_UART_Receive_IT(&huart6, character, 1) == HAL_OK) {
	 buffer[i - 1] = *character;

	 // add a delemiter or value
	 if (*character == 74) {
		 	 if (buffer[i-1]== 95)
		 	 {
		 		 uint8_t resetLora[] = { 0x41,0x54,0x5A, 0x0d, 0x0a};
		 		 HAL_UART_Transmit(&huart6, resetLora, 5, HAL_MAX_DELAY);
		 		 HAL_Delay(3000);
		 		 initLora();
		 	 }
			 HAL_UART_Transmit(&huart2, "JOINED\n",7, HAL_MAX_DELAY);
			 i = 0;
			 loraJoined = true;
		 }

	 }

		i = i + 1;

	 }
	 return;


	 }



void initGPS(){
	uint8_t ggaOff[] = {0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x31, 0x2a, 0x32, 0x34, 0x0d, 0x0a};
	HAL_UART_Transmit(&huart1, ggaOff, 25, HAL_MAX_DELAY);
	uint8_t gglOn[] = {0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2c, 0x30, 0x31, 0x2c, 0x30, 0x30, 0x2c, 0x36, 0x30, 0x2c, 0x30, 0x31, 0x2a, 0x32, 0x33, 0x0d, 0x0a};
	HAL_UART_Transmit(&huart1, gglOn, 25, HAL_MAX_DELAY);
	uint8_t gsaOff[] = {0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2c, 0x30, 0x32, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x31, 0x2a, 0x32, 0x36, 0x0d, 0x0a};
	HAL_UART_Transmit(&huart1, gsaOff, 25, HAL_MAX_DELAY);
	uint8_t gsvOff[] = {0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2c, 0x30, 0x33, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x31, 0x2a, 0x32, 0x37, 0x0d, 0x0a};
	HAL_UART_Transmit(&huart1, gsvOff, 25, HAL_MAX_DELAY);
	uint8_t rmcOff[] = {0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2c, 0x30, 0x34, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x31, 0x2a, 0x32, 0x30, 0x0d, 0x0a};
	HAL_UART_Transmit(&huart1, rmcOff, 25, HAL_MAX_DELAY);
	uint8_t vtgOff[] = {0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2c, 0x30, 0x35, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x31, 0x2a, 0x32, 0x31, 0x0d, 0x0a};
	HAL_UART_Transmit(&huart1, vtgOff, 25, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2,"GPS Set!\n", sizeof("GPS Set!\n")-1, HAL_MAX_DELAY);


}
void sendGPS() {
	uint8_t buffer[64];
	uint8_t character[1];
	int i = 0;
	int k = 0;
	//__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);

	while (dangerZone) {
		//when there is a uart interrupt check the character and put it in the buffer.
		//If the character is the end delemiter the NMEA command is complete.

		if (HAL_UART_Receive_IT(&huart1, character, 1) == HAL_OK) {
			buffer[i - 1] = *character;
			if (*character == '\n') {

				//AT COMMAND: AT+SEND=2:<data><CR><LF>
				uint8_t AT_COMMAND[] = { 0x41, 0x54, 0x2b, 0x53, 0x45, 0x4e,
						0x44, 0x3d, 0x32, 0x3a };
				for (int j = 0; j <= i; j++) {
					AT_COMMAND[j + 10] = buffer[j];
				}

				//end delemiter CR and LF
				AT_COMMAND[i + 11] = 0x0d;
				AT_COMMAND[i + 12] = 0x0a;

				HAL_UART_Transmit(&huart6, AT_COMMAND, i + 10, HAL_MAX_DELAY);
				i = 0;
				k = k+1;

			}
			i = i + 1;

		}


}
	return;
}



/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 4800;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART6 init function */
static void MX_USART6_UART_Init(void)
{

  huart6.Instance = USART6;
  huart6.Init.BaudRate = 9600;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

}

/** Pinout Configuration
*/
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
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
