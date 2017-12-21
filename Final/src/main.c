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

//#include "global.h"

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart4;
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
static void MX_UART4_Init(void);

/*
 *  The different states:
 *  	- safe_zone: the mobile node is in normal
 *  	mode, sending sensor data over DAHS7
 *
 *		- in_danger_zone: mobile node is outside.
 *		The GSP and Lora module are powered and the
 *		lora is initialzed
 *
 *		- alarm, when clocked on the panic button
 *
 *		- lora_gps_powered
 *
 *
 *
 * */

int main(void) {

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
	MX_UART4_Init();

	loraTries = 0;
	lora_gps_powered = false;
	state = in_danger_zone;
	//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7); //power Lora and GPS-module

	/* Initialize all sensors */
	setBarInterface(&hi2c1, &huart2);

	HAL_UART_Transmit(&huart2, "Starting...\n", 11, HAL_MAX_DELAY);

	HAL_Delay(5000);

	while (1) {

		switch (state) {

		case safe_zone:
			readsomething();
			break;
		case lora_ready:
			initGPS();
			HAL_Delay(500);
			HAL_UART_Transmit(&huart3, "Danger zone!\n",
					sizeof("Danger zone!\n") -1, HAL_MAX_DELAY);

			state = lora_sending;
			break;
		case in_danger_zone:

			HAL_UART_Transmit(&huart2, "Danger zone!\n",
					sizeof("Danger zone!\n") -1, HAL_MAX_DELAY);
			if (lora_gps_powered == false) {
				HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7); //power Lora and GPS-module
				lora_gps_powered = true;
			}
			initLora();


			HAL_UART_Transmit(&huart2, loraTries,
								1, HAL_MAX_DELAY);
			loraTries = loraTries + 1;
			if (loraTries >= 5) {
				state = alarm_state;
			}

			break;
		case alarm_state:
			HAL_UART_Transmit(&huart2, "Alarm\n", sizeof("Alarm\n")-1,
					HAL_MAX_DELAY);
			loraError();
			HAL_Delay(2000);
			break;

		case lora_sending:
			sendGPS();
			break;


		}
	}
}

/* Initializes LoRa Module */
void initLora() {
	uint8_t init[4];
	HAL_UART_Transmit(&huart2, "Init Lora\n", 10, HAL_MAX_DELAY);

	/* Do until received an OK, after 10 times, send ERROR over Dash7*/

		//reset the LoRa Module: <ATZ>
		uint8_t reset_cmd[] = { 0x41, 0x54, 0x5A, 0x0d, 0x0a };
		HAL_UART_Transmit(&huart3, reset_cmd, 5, HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, "RESET\n", 6, HAL_MAX_DELAY);

		//Delay until the module is reset
		HAL_Delay(2000);

		//Sent message has not to be confirmed
		uint8_t confirm_cmd[] = { 0x41, 0x54, 0x2b, 0x43, 0x46, 0x4d, 0x3d,
				0x31, 0x0d, 0x0a };
		HAL_UART_Transmit(&huart3, confirm_cmd, 10, HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, "CONFIRM MODE\n", 13, HAL_MAX_DELAY);
		HAL_Delay(2000);

		//flush the UART register
		__HAL_UART_FLUSH_DRREGISTER(&huart3);

		// AT command to join a network: <AT+JOIN>
		uint8_t join_cmd[] = { 0x41, 0x54, 0x2b, 0x4a, 0x4f, 0x49, 0x4e, 0x0d,
				0x0a };

		HAL_UART_Transmit(&huart3, join_cmd, 9, HAL_MAX_DELAY);

		/* Check if the previous command has arrived */
		HAL_UART_Receive(&huart3, init, 4, HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart2, "No or poor connection with Lora Module\n",
				39, HAL_MAX_DELAY);

		if (init[2] == 'O' && init[3] == 'K') {

			HAL_UART_Transmit(&huart2, "TRY JOINING...\n", 15, HAL_MAX_DELAY);
			checkNetwork();

		}

		/* When AT+COMMANDS are not getting an OK -> alarm state */


}

/* Check if a LoRa network is joined */
void checkNetwork() {
	// Delay to make sure the end node had enough time to join
	uint8_t at_joined[1];
	int check_joined = 0;
	HAL_Delay(1000);

	HAL_UART_Transmit(&huart2, "Check Connection\n",
					sizeof("Check Connection\n")-1, HAL_MAX_DELAY);

	while (check_joined <40) {

		// Check if joined: <AT+NJS=?>
		uint8_t checkNetwork[] = { 0x41, 0x54, 0x2b, 0x4e, 0x4a, 0x53, 0x3d,
				0x3f, 0x0d, 0x0a };
		HAL_UART_Transmit(&huart3, checkNetwork, 10, HAL_MAX_DELAY);


		if (HAL_UART_Receive_IT(&huart3, at_joined, 1) == HAL_OK) {


			if (*at_joined == '1') {

				HAL_UART_Transmit(&huart2, "JOINED!\n", 8, HAL_MAX_DELAY);
				state = lora_ready;
				//return;

			}


		if (*at_joined == '0') {
				check_joined = check_joined + 1;
				}

			}
		}
						HAL_UART_Transmit(&huart2, "COULD NOT JOIN, RESET!\n", 21, HAL_MAX_DELAY);
	}



/* Function that sends an error message over Dash7 to the backend */
void loraError() {
	HAL_UART_Transmit(&huart2, "JOIN ERROR\n", 11, HAL_MAX_DELAY);
	uint8_t join_error[] = { 0x65, 0x72, 0x72, 0x6f, 0x72 };
	DASH7Message(join_error, 5);
}

/* Initializes the GPS module */
void initGPS() {
	uint8_t gga_off[] = { 0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2c,
			0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x31,
			0x2a, 0x32, 0x34, 0x0d, 0x0a };
	HAL_UART_Transmit(&huart1, gga_off, 25, HAL_MAX_DELAY);
	// NMAE cmd to receive every minute
	uint8_t ggl_on[] = { 0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2c,
			0x30, 0x31, 0x2c, 0x30, 0x30, 0x2c, 0x36, 0x30, 0x2c, 0x30, 0x31,
			0x2a, 0x32, 0x33, 0x0d, 0x0a };
	HAL_UART_Transmit(&huart1, ggl_on, 25, HAL_MAX_DELAY);
	uint8_t gsa_off[] = { 0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2c,
			0x30, 0x32, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x31,
			0x2a, 0x32, 0x36, 0x0d, 0x0a };
	HAL_UART_Transmit(&huart1, gsa_off, 25, HAL_MAX_DELAY);
	uint8_t gsv_off[] = { 0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2c,
			0x30, 0x33, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x31,
			0x2a, 0x32, 0x37, 0x0d, 0x0a };
	HAL_UART_Transmit(&huart1, gsv_off, 25, HAL_MAX_DELAY);
	uint8_t rmc_off[] = { 0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2c,
			0x30, 0x34, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x31,
			0x2a, 0x32, 0x30, 0x0d, 0x0a };
	HAL_UART_Transmit(&huart1, rmc_off, 25, HAL_MAX_DELAY);
	uint8_t vtg_off[] = { 0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2c,
			0x30, 0x35, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x31,
			0x2a, 0x32, 0x31, 0x0d, 0x0a };
	HAL_UART_Transmit(&huart1, vtg_off, 25, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, "GPS is initialized!\n", sizeof("GPS is initialized!\n") - 1,
	HAL_MAX_DELAY);

}

void sendGPS() {
	uint8_t buffer[64];
	uint8_t character[1];
	int i = 0;


	while (1) {
		/* when there is a UART receive interrupt, put the character in the buffer.
		 * If the character is the end delimiter the NMEA command is completed.
		 * The first part: <$GPGLL,> of the buffer is deleted before sending  */
		if (HAL_UART_Receive_IT(&huart1, character, 1) == HAL_OK) {
			buffer[i - 1] = *character;


			//
			/* check for the end of the GLL sentence and check if it's a GLL sentence */
			if (*character == '\n') {

				//AT COMMAND: AT+SEND=2:<gps data><CR><LF>
				uint8_t AT_COMMAND[] = { 0x41, 0x54, 0x2b, 0x53, 0x45, 0x4e,
						0x44, 0x3d, 0x32, 0x3a };
				for (int j = 0; j <= i; j++) {
					AT_COMMAND[10 + j] = buffer[j];
				}

				// Add <CR><LF>
				AT_COMMAND[i + 11] = 0x0d;
				AT_COMMAND[i + 12] = 0x0a;

				// Send over LoRa
				HAL_UART_Transmit(&huart2, AT_COMMAND, i + 10, HAL_MAX_DELAY);
				HAL_UART_Transmit(&huart3, AT_COMMAND, i + 10, HAL_MAX_DELAY);
				i = 0;
			}
			i = i + 1;

		}

	}
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7); //Toggle the state of pin PC9

	return;
}

/* Function to send Dash7 messages with a variable length */
void DASH7Message(uint8_t data[], int lengthDash7) {

	uint8_t CMD[] = { 0x32, 0xd7, 0x01, 0x00, 0x10, 0x01, //FORWARD ACTION
			0x20, 0x01, 0x00, //Return File Data, File ID, Offset
			lengthDash7, //length from the data
			};

	int sizeCMD = sizeof(CMD);

	//determine the ALP Command length
	int length = FRAME_LENGTH + sizeCMD - lengthDash7 - 1;

	uint8_t FRAME[] = { 0x41, 0x54, 0x24, 0x44, 0xc0, 0x00, // SERIAL INTERFACE
			length, };

	//create the ALP array with the right length
	uint8_t ALP[sizeCMD + FRAME_LENGTH + lengthDash7];

	for (int i = 0; i < FRAME_LENGTH + 1; i++)
		ALP[i] = FRAME[i];
	for (int j = 0; j < sizeCMD + 1; j++)
		ALP[j + FRAME_LENGTH] = CMD[j];

	//put the data in de ALP command
	for (int k = 0; k < lengthDash7; k++) {
		//ALP length is 16 and + 1 for lengthDash7 field
		ALP[17 + k] = data[k];
	}

	HAL_UART_Transmit(&huart4, ALP, sizeof(ALP), HAL_MAX_DELAY);
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
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
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

/* UART4 init function -> used for the DASH7 module */
static void MX_UART4_Init(void) {

	huart4.Instance = UART4;
	huart4.Init.BaudRate = 115200;
	huart4.Init.WordLength = UART_WORDLENGTH_8B;
	huart4.Init.StopBits = UART_STOPBITS_1;
	huart4.Init.Parity = UART_PARITY_NONE;
	huart4.Init.Mode = UART_MODE_TX_RX;
	huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart4.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart4) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* USART1 init function -> used for the GPS with a baudrate of 4800 */

static void MX_USART1_UART_Init(void) {

	huart1.Instance = USART1;
	huart1.Init.BaudRate = 4800;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* USART2 init function -> used for printing*/
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

/* USART3 init function -> used for LoRa module*/
static void MX_USART3_UART_Init(void) {

	huart3.Instance = USART3;
	huart3.Init.BaudRate = 9600;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart3) != HAL_OK) {
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
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

	/*Configure GPIO pin : PC13 */
	GPIO_InitStruct.Pin = GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

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

void _Error_Handler(char * file, int line) {

	while (1) {
	}
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
