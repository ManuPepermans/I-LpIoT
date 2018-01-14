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

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;

/* WWDG handler declaration */
WWDG_HandleTypeDef WwdgHandle;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_UART4_Init(void);
static void MX_UART5_Init(void);

/*Buzzer functions */
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/*
 *  The different states:
 *  	- safe_zone: the mobile node is in normal
 *  	mode, sending sensor data over DASH7, when receiving a uart interrupt
 *  	go to state: in_danger_zone
 *
 *		- in_danger_zone: mobile node is outside.
 *		The GSP and Lora module are powered and the
 *		lora is initialised. If the connection with the lora module is not working
 *		there will be an alarm after several tries.
 *
 *		- lora_ready: Lora is ready, so initialize the GPS
 *
 *		- alarm_state: sending an alarm message over DASH7
 *			* cannot join Lora
 *			* panic button
 *			* buzzer
 *
 * */

int main(void) {

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/*##-1- Check if the system has resumed from WWDG reset ####################*/
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET) {
		/* WWDGRST flag set*/
		HAL_UART_Transmit(&huart2, "WWDGRST FLAG SET\n",
				sizeof("WWDGRST FLAG SET\n") - 1, HAL_MAX_DELAY);

		/* Clear reset flags */
		__HAL_RCC_CLEAR_RESET_FLAGS();
	} else {
		/* WWDGRST flag is not set*/
		HAL_UART_Transmit(&huart2, "WWDGRST FLAG NOT SET\n",
				sizeof("WWDGRST FLAG NOT SET\n") - 1, HAL_MAX_DELAY);
	}

	/*##-2- Configure the WWDG peripheral ######################################*/
	/* WWDG clock counter = (PCLK1 (2MHz)/4096)/8) = 61 Hz (0.01639 s)
	 WWDG Window value = 80 means that the WWDG counter should be refreshed only
	 when the counter is below 254 (and greater than 64) otherwise a reset will
	 be generated.
	 WWDG Counter value = 255, WWDG timeout = ~0.01639s*64 = 1.04896 s


	 This means: WWDG starts counting at 255*0.01639s=4.17945s, reset has to be between
	 ~0.01639s*254=4.16306s and ~0.01639s*64=1.04896s.								*/
	WwdgHandle.Instance = WWDG;

	WwdgHandle.Init.Prescaler = WWDG_PRESCALER_8;
	WwdgHandle.Init.Window = 254;
	WwdgHandle.Init.Counter = 255;

	/* Initialize booleans*/
	loraTries = 0;
	lora_gps_powered = false;

	/* Define the state to start with*/
	state = safe_zone;

	// define some variables
	int message;
	int j;
	j = 0;

	HAL_Delay(5000);

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();
	MX_I2C1_Init();
	MX_UART4_Init();
	MX_UART5_Init();

	MX_GPIO_Init();
	MX_TIM2_Init();

	//PWM timer Initialize
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

	HAL_UART_Transmit(&huart2, "Starting ElderTrack...\n",
			sizeof("Starting ElderTrack...\n") - 1, HAL_MAX_DELAY);

	/*##-5- Start the WWDG #####################################################*/
	if (HAL_WWDG_Init(&WwdgHandle) != HAL_OK) {
		Error_Handler();
	}

	int i;
	uint8_t buffer[200];
	uint8_t D7Rx[1];
//power dash7
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
	HAL_Delay(3000);

	/* Infinite loop */
	while (1) {
		//HAL_Delay(1000);
		switch (state) {

		//Check on '1', if '1' present enter dash7 state
		// While checking for '1' read sensors and send over dash7
		case safe_zone:
			j = j + 1;
			__HAL_UART_FLUSH_DRREGISTER(&huart4);
			if (HAL_UART_Receive_IT(&huart4, D7Rx, 1) == HAL_OK) {
				buffer[i] = *D7Rx;
				if (buffer[i] == '1') {
					state = dash7_downlink;
					message = 1;
					__HAL_UART_FLUSH_DRREGISTER(&huart4);
				}
			}
			if (j > 100000) {
				getAndSendAltitude();
				j = 0;

			}

			/* Refresh WWDG: update counter value to 255, he refresh window is:
			 ~780 * (127-80) = 36.6ms < refresh window < ~780 * 64 = 49.9ms */

			resetWWDG();
			break;
		case in_danger_zone:

			HAL_UART_Transmit(&huart2, "Danger zone!\n",
					sizeof("Danger zone!\n") - 1, HAL_MAX_DELAY);
			// toggle when LoRa is not powered
			if (lora_gps_powered == false) {
				HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7); //power Lora and GPS-module
				lora_gps_powered = true;
			}
			initLora();
			resetWWDG();
			// when tried 5 times connecting to LoRa -> alarm!
			loraTries = loraTries + 1;
			if (loraTries >= 5) {
				state = alarm_state;
			}
			resetWWDG();

			break;
		case lora_ready:
			// delay could be less, GPS must be running
			HAL_Delay(5000);
			initGPS();
			state = lora_sending;
			resetWWDG();
			break;

		case alarm_state:
			HAL_UART_Transmit(&huart2, "Alarm\n", sizeof("Alarm\n") - 1,
			HAL_MAX_DELAY);
			BuzzerAlert();
			loraError();
			HAL_Delay(2000);
			resetWWDG();
			break;

		case lora_sending:
			sendGPS();
			resetWWDG();
			break;

			// When dash7 downlink, message will be cheched to check what to do
		case dash7_downlink:
			switch (message) {
			case 1:
				state = in_danger_zone;
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			}
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
	uint8_t confirm_cmd[] = { 0x41, 0x54, 0x2b, 0x43, 0x46, 0x4d, 0x3d, 0x31,
			0x0d, 0x0a };
	HAL_UART_Transmit(&huart3, confirm_cmd, 10, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, "CONFIRM MODE\n", 13, HAL_MAX_DELAY);
	HAL_Delay(2000);

	//flush the UART register
	__HAL_UART_FLUSH_DRREGISTER(&huart3);

	// AT command to join a network: <AT+JOIN>
	uint8_t join_cmd[] =
			{ 0x41, 0x54, 0x2b, 0x4a, 0x4f, 0x49, 0x4e, 0x0d, 0x0a };

	HAL_UART_Transmit(&huart3, join_cmd, 9, HAL_MAX_DELAY);

	/* Check if the previous command has arrived */
	HAL_UART_Receive(&huart3, init, 4, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, "N0 OR POOR CONNECTION WITH MODULE, TRY AGAIN\n",
			sizeof("N0 OR POOR CONNECTION WITH MODULE, TRY AGAIN\n") - 1,
			HAL_MAX_DELAY);

	if (init[2] == 'O' && init[3] == 'K') {

		HAL_UART_Transmit(&huart2, "TRY JOINING...\n", 15, HAL_MAX_DELAY);
		checkNetwork();

	} else {
		HAL_UART_Transmit(&huart2,
				"N0 OR POOR CONNECTION WITH MODULE, TRY AGAIN\n",
				sizeof("N0 OR POOR CONNECTION WITH MODULE, TRY AGAIN\n") - 1,
				HAL_MAX_DELAY);
	}

	/* When AT+COMMANDS are not getting an OK -> alarm state */

}

/* Check if a LoRa network is joined */
void checkNetwork() {
	// Delay to make sure the end node had enough time to join
	uint8_t at_joined[1];
	int check_joined = 0;
	bool joined_lora;

	HAL_Delay(8000);

	while (check_joined < 50) {
		// Check if joined: <AT+NJS=?>
		uint8_t checkNetwork[] = { 0x41, 0x54, 0x2b, 0x4e, 0x4a, 0x53, 0x3d,
				0x3f, 0x0d, 0x0a };
		HAL_UART_Transmit(&huart3, checkNetwork, 10, HAL_MAX_DELAY);

		if (HAL_UART_Receive_IT(&huart3, at_joined, 1) == HAL_OK) {

			if (*at_joined == '1') {

				HAL_UART_Transmit(&huart2, "JOINED!\n", 8, HAL_MAX_DELAY);
				state = lora_ready;
				return;

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
	uint8_t join_error[1] = { 0x45 };

	DASH7Message(join_error, 1);
}

/* Initializes the GPS module */
void initGPS() {
	uint8_t gga_off[] = { 0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2c,
			0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x31,
			0x2a, 0x32, 0x34, 0x0d, 0x0a };
	HAL_UART_Transmit(&huart5, gga_off, 25, HAL_MAX_DELAY);
	// NMAE cmd to receive every minute
	uint8_t ggl_on[] = { 0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2c,
			0x30, 0x31, 0x2c, 0x30, 0x30, 0x2c, 0x36, 0x30, 0x2c, 0x30, 0x31,
			0x2a, 0x32, 0x33, 0x0d, 0x0a };
	HAL_UART_Transmit(&huart5, ggl_on, 25, HAL_MAX_DELAY);
	uint8_t gsa_off[] = { 0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2c,
			0x30, 0x32, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x31,
			0x2a, 0x32, 0x36, 0x0d, 0x0a };
	HAL_UART_Transmit(&huart5, gsa_off, 25, HAL_MAX_DELAY);
	uint8_t gsv_off[] = { 0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2c,
			0x30, 0x33, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x31,
			0x2a, 0x32, 0x37, 0x0d, 0x0a };
	HAL_UART_Transmit(&huart5, gsv_off, 25, HAL_MAX_DELAY);
	uint8_t rmc_off[] = { 0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2c,
			0x30, 0x34, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x31,
			0x2a, 0x32, 0x30, 0x0d, 0x0a };
	HAL_UART_Transmit(&huart5, rmc_off, 25, HAL_MAX_DELAY);
	uint8_t vtg_off[] = { 0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2c,
			0x30, 0x35, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x31,
			0x2a, 0x32, 0x31, 0x0d, 0x0a };
	HAL_UART_Transmit(&huart5, vtg_off, 25, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, "GPS is initialized!\n",
			sizeof("GPS is initialized!\n") - 1,
			HAL_MAX_DELAY);

}

void BuzzerAlert(void) {
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 154);
	HAL_Delay(200);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 0);
	HAL_Delay(20);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 154);
	HAL_Delay(200);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 0);
	HAL_Delay(29600);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 154);
	HAL_Delay(200);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 0);
	HAL_Delay(20);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 154);
	HAL_Delay(200);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, 0);
	return;
}

void resetWWDG(void) {
	HAL_WWDG_Refresh(&WwdgHandle);
}

void sendGPS() {
	uint8_t buffer[64];
	uint8_t character[1];
	int i = 0;
	__HAL_UART_FLUSH_DRREGISTER(&huart5);

	while (1) {
		/* when there is a UART receive interrupt, put the character in the buffer.
		 * If the character is the end delimiter the NMEA command is completed.
		 * The first part: <$GPGLL,> of the buffer is deleted before sending  */
		if (HAL_UART_Receive_IT(&huart5, character, 1) == HAL_OK) {
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
				BuzzerAlert();
			}

			i = i + 1;

		}
	}
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7); //Toggle the state of pin PC9

	return;
}

/* Function to send Dash7 messages with a variable length
 * 0x41 0x54 0x24 0x44 0xc0 0x00 0x0d
 * 0xb4 0x13 0x32 0xd7 0x00 0x00 0x10 0x01 0x20 0x40
 * 0x00 0x01 0x01
 *
 * */
void DASH7Message(uint8_t data[], int lengthDash7) {
	int total_length = CMD_LENGTH - 7 + lengthDash7;
	uint8_t CMD[] = { 0x41, 0x54, 0x24, 0x44, 0xc0, 0x00, total_length, 0xb4,
			0x13, 0x32, 0xd7, 0x00, 0x00, 0x10, 0x01, 0x20, 0x40, 0x00,
			lengthDash7, };

	//create the ALP array with the right length
	uint8_t ALP[total_length + 7];

	for (int i = 0; i <= CMD_LENGTH; i++) {
		ALP[i] = CMD[i];
	}

	//put the data in de ALP command
	for (int k = 0; k <= lengthDash7; k++) {
		//ALP length is 16 and + 1 for lengthDash7 field
		ALP[CMD_LENGTH + k] = data[k];
	}

	HAL_UART_Transmit(&huart4, ALP, sizeof(ALP), HAL_MAX_DELAY);

}

void getAndSendAltitude() {
	//Set one shot mode for barometer for low power consumption
	Settings = LPS22HB_ONE__SHOT_ENABLE;
	HAL_I2C_Mem_Write(&hi2c1, LPS22HB_BARO_ADDRESS, LPS22HB_CTRL_REG2, 1,
			&Settings, 1, 100);
	//Read barometer dataRegister(LSB) and print
	HAL_I2C_Mem_Read(&hi2c1, LPS22HB_BARO_ADDRESS, LPS22HB_PRESS_OUT_XL, 1,
			&LSBpressure, 1, 100);
	//Read barometer dataRegister(MID) and print
	HAL_I2C_Mem_Read(&hi2c1, LPS22HB_BARO_ADDRESS, LPS22HB_OUT_L, 1,
			&MIDpressure, 1, 100);
	//Read barometer dataRegister(MSB) and print
	HAL_I2C_Mem_Read(&hi2c1, LPS22HB_BARO_ADDRESS, LPS22HB_OUT_H, 1,
			&MSBpressure, 1, 100);
	//Read temperature dataRegister(LSB) and print
	HAL_I2C_Mem_Read(&hi2c1, LPS22HB_BARO_ADDRESS, LPS22HB_TEMP_OUT_L, 1,
			&LSBtemp, 1, 100);
	//Read temperature dataRegister(MSB) and print
	HAL_I2C_Mem_Read(&hi2c1, LPS22HB_BARO_ADDRESS, LPS22HB_TEMP_OUT_H, 1,
			&MSBtemp, 1, 100);
	temperature = (((uint16_t) MSBtemp << 8 | LSBtemp));
	int temperature2 = ((int) temperature) / 100;
	if (temperature2 > 30 || temperature2 < 15) {
		// TODO buzzerAlert();
	}
	uint8_t dataToSend[] = { 0x42, MSBpressure, MIDpressure, LSBpressure,
			MSBtemp, LSBtemp };
	DASH7Message(dataToSend, 6);
	HAL_UART_Transmit(&huart2, "Sensor used!\n", sizeof("Sensor used!\n") - 1,
			HAL_MAX_DELAY);

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

/* UART4 init function */
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

/* GPS UART*/
static void MX_UART5_Init(void) {

	huart5.Instance = UART5;
	huart5.Init.BaudRate = 4800;
	huart5.Init.WordLength = UART_WORDLENGTH_8B;
	huart5.Init.StopBits = UART_STOPBITS_1;
	huart5.Init.Parity = UART_PARITY_NONE;
	huart5.Init.Mode = UART_MODE_TX_RX;
	huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart5.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart5) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* Debug UART */
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

/* USART3 init function */
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

static void MX_TIM2_Init(void) {

	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_OC_InitTypeDef sConfigOC;

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 24;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 200;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)
			!= HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2)
			!= HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	HAL_TIM_MspPostInit(&htim2);

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
	__HAL_RCC_GPIOD_CLK_ENABLE()
	;

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

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

	/*Configure GPIO pin : LD2_Pin */
	GPIO_InitStruct.Pin = LD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : PB6 */
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : PB3 */
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

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
