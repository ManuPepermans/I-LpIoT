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
#include <stdio.h>
#define LENGTH_ARRAY(x)  (sizeof(x) / sizeof(uint8_t))
#define FRAME_LENGTH 7
#define SERIAL 7
#define BUFSIZE 200

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_I2C1_Init(void);
static void sendGPS(void);
static void initLora(void);
static void initGPS(void);
void DASH7Message(uint8_t[], int);


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
  MX_USART3_UART_Init();
  MX_I2C1_Init();

  HAL_Delay(5000);

  /* Infinite loop */
  while (1)
  {
	  for(int i = 0; i <= 10; i++){
	  uint8_t uartTest [] = {0x4d, 0x01, 0x04};
	  int arrayLength = LENGTH_ARRAY(uartTest);
	  DASH7Message(uartTest, arrayLength);
	  HAL_Delay(2000);
	  }

	dangerZone = true;
	if(dangerZone == true)
	{
		HAL_UART_Transmit(&huart2,"Danger zone!\n", sizeof("Danger zone!\n")-1, HAL_MAX_DELAY);
	initGPS();
	HAL_Delay(1000);
	initLora();}
	while (dangerZone) {

		sendGPS();
  }
}
}


  void initLora() {
  	uint8_t buffer[64];
  	uint8_t character[1];
  	int i = 0;

  	 uint8_t joinLora[] = {0x41,0x54,0x2b,0x4a,0x4f,0x49,0x4e, 0x0d, 0x0a};
  	 HAL_UART_Transmit(&huart3, joinLora, 9, HAL_MAX_DELAY);
  	 bool loraJoined = false;
  	 while(loraJoined == false){

  	 if (HAL_UART_Receive_IT(&huart3, character, 1) == HAL_OK) {
  	 buffer[i - 1] = *character;

  	 // add a delemiter or value
  	 if (*character == 74) {
  		 	 if (buffer[i-1]== 95)
  		 	 {
  		 		 uint8_t resetLora[] = { 0x41,0x54,0x5A, 0x0d, 0x0a};
  		 		 HAL_UART_Transmit(&huart3, resetLora, 5, HAL_MAX_DELAY);
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
  				HAL_UART_Transmit(&huart2, AT_COMMAND, i + 10, HAL_MAX_DELAY);

  				HAL_UART_Transmit(&huart3, AT_COMMAND, i + 10, HAL_MAX_DELAY);
  				i = 0;
  				k = k+1;

  			}
  			i = i + 1;

  		}


  }
  	return;
}

void DASH7Message(uint8_t data[], int lengthDash7)
{

	uint8_t CMD[] = {
			0x32, 0xd7, 0x01, 0x00, 0x10, 0x01, //FORWARD ACTION
			0x20, 0x01, 0x00, //Return File Data, File ID, Offset
			lengthDash7, //length from the data
	};

	int sizeCMD = sizeof(CMD);

	//determine the ALP Command length
	int length = FRAME_LENGTH + sizeCMD - lengthDash7 -1;

	uint8_t FRAME[] = {
				0x41, 0x54, 0x24, 0x44, 0xc0, 0x00, // SERIAL INTERFACE
				length ,
	 };

	//create the ALP array with the right length
	uint8_t ALP[sizeCMD+FRAME_LENGTH+lengthDash7];

	for(int i = 0; i<FRAME_LENGTH+1; i++)
		ALP[i] = FRAME[i];
	for(int j = 0; j<sizeCMD+1; j++)
		 ALP[j+FRAME_LENGTH] = CMD[j];


	//put the data in de ALP command
	for(int k = 0; k < lengthDash7; k++)
	{
			//ALP length is 16 and + 1 for lengthDash7 field
	          ALP[17+k] = data[k];
	      }

HAL_UART_Transmit(&huart2, ALP, sizeof(ALP),HAL_MAX_DELAY);
}





/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
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

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

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
  huart2.Init.BaudRate = 115200;
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

/* USART3 init function */
static void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
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
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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

void assert_failed(uint8_t* file, uint32_t line)
{

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
