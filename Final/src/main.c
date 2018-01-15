/**
 ******************************************************************************
 * @name main.c
 * @brief this is the main program body for the project. The major part of the code exist of a Finite State Machine (FSM):
 * - Safe Zone: The patient is in the predefined safe zone. The sensor data (barometer & ecompass) are send over DASH7 and when an interrupt is
 * received over DASH7 the state will switch to the DASH7 Downlink state.
 * - DASH7 Downlink: the next state will be decided on the received message. For now only 1 type of message is supported but this will be expand in the future.
 * - Danger Zone: The patient is outside the predefined safe zone. The LORA and GPS module are turned on and depending on whether the system could connect to LORA
 * the systems goes in Alarm state or to the Lora Ready state.
 * - Alarm State: If the system couldn't connect to the LORA an alarm message over DASH7 will be send  and a buzzer will go off.
 * - Lora Ready: LORA is ready and the GPS will be initialized.
 * - Lora Sending: Sending the GPS coordinates of the patient over LORA.
 * @dot
 * digraph FSM {
 * node [shape = box];
 * SafeZone    [fillcolor=lightblue,style=filled,label="SafeZone" ];
 * DangerZone [fillcolor=lightblue,style=filled,label="DangerZone" ];
 * AlarmState [fillcolor=lightblue,style=filled,label="AlarmState"];
 * LoraReady [fillcolor=lightblue,style=filled,label="LoraReady"];
 * LoraSending [fillcolor=lightblue,style=filled,label="LoraSending"];
 * DASH7Downlink [fillcolor=lightblue,style=filled,label="DASH7Downlink"];
 * Future [fillcolor=lightblue,style=filled,label="Future"];
 *
 * SafeZone -> SafeZone;
 * SafeZone -> DASH7Downlink;
 * DASH7Downlink -> DangerZone;
 * DASH7Downlink -> Future;
 * DangerZone -> AlarmState;
 * AlarmState -> AlarmState;
 * DangerZone -> LoraReady;
 * LoraReady -> LoraSending;
 * }
 * @enddot
 *
 * @authors Imad Bouhadan, Dellafaille Lander, Marien Levi, Pepermans Manu, Van de Mieroop Kevin
 ******************************************************************************
 */

/* Includes */
#include "main.h"
#include "stm32l1xx_hal.h"

/* Handler declaration */
I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim2;
WWDG_HandleTypeDef WwdgHandle;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* Private function prototypes */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_UART4_Init(void);
static void MX_UART5_Init(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/**
 * @brief The main function of the program. The peripherals are initialized and than the program enters a finite state machine which was defined previously.
 * @return
 */
int main(void) {

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Check if the system has resumed from WWDG reset */
	if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET) {
		/* WWDGRST flag is set */
		HAL_UART_Transmit(&huart2, "WWDGRST FLAG SET\n",
				sizeof("WWDGRST FLAG SET\n") - 1, HAL_MAX_DELAY);

		/* Clear reset flags */
		__HAL_RCC_CLEAR_RESET_FLAGS();
	} else {
		/* WWDGRST flag is not set */
		HAL_UART_Transmit(&huart2, "WWDGRST FLAG NOT SET\n",
				sizeof("WWDGRST FLAG NOT SET\n") - 1, HAL_MAX_DELAY);
	}

	/*
	 * Configure the WWDG peripheral
	 * WWDG clock counter = (PCLK1 (2MHz)/4096)/8) = 61 Hz (0.01639 s)
	 * WWDG Window value = 80 means that the WWDG counter should be refreshed only
	 * when the counter is below 254 (and greater than 64) otherwise a reset will
	 * be generated. WWDG Counter value = 255, WWDG timeout = ~0.01639s*64 = 1.04896 s
	 *
	 * This means: WWDG starts counting at 255*0.01639s=4.17945s, reset has to be between
	 * ~0.01639s*254=4.16306s and ~0.01639s*64=1.04896s.
	 */
	WwdgHandle.Instance = WWDG;

	WwdgHandle.Init.Prescaler = WWDG_PRESCALER_8;
	WwdgHandle.Init.Window = 254;
	WwdgHandle.Init.Counter = 255;

	/* Initialize booleans */
	loraTries = 0;
	lora_gps_powered = false;

	/* Define the state to start with */
	state = safe_zone;

	/* Define some variables */
	int message;
	int j;
	j = 0;
	int i;
	uint8_t buffer[200];
	uint8_t D7Rx[1];

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

	//LSM303AGR_init();

	/* PWM Timer initialization */
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

	HAL_UART_Transmit(&huart2, "Starting ElderTrack...\n",
			sizeof("Starting ElderTrack...\n") - 1, HAL_MAX_DELAY);

	/* Start the WWDG */
	if (HAL_WWDG_Init(&WwdgHandle) != HAL_OK) {
		Error_Handler();
	}

	/* Power of DASH7 */
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
	HAL_Delay(3000);

	while (1) {

		//HAL_Delay(1000);
		switch (state) {

		/* Check on '1', if '1' present enter DASH7 state.
		 * While checking for '1' read sensors and send over dash7 */
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

//			if (j == 50000) {
//				getAndSendAltitude();
//			} else if (j > 100000) {
//				ecompassAlgorithm();
//				j = 0;
//			}

			/* Refresh WWDG: update counter value to 255, he refresh window is:
			 ~780 * (127-80) = 36.6ms < refresh window < ~780 * 64 = 49.9ms */
			resetWWDG();
			break;

		case in_danger_zone:
			HAL_UART_Transmit(&huart2, "Danger zone!\n",
					sizeof("Danger zone!\n") - 1, HAL_MAX_DELAY);
			/* Toggle the pin when LORA is not powered */
			if (lora_gps_powered == false) {
				/* Power LORA and the GPS module */
				HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
				lora_gps_powered = true;
			}
			initLora();
			resetWWDG();
			/* Try to connect 5 times to LORA, if it's not successful enter alarm state */
			loraTries = loraTries + 1;
			if (loraTries >= 5) {
				state = alarm_state;
			}
			resetWWDG();

			break;

		case lora_ready:
			/* Delay introduced to ensure GPS is up and running */
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

			/* When a DASH7 downlink message arrives, check the message to now what to do */
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

/**
 * @brief Function for initializing the LORA module.
 */
void initLora() {
	uint8_t init[4];
	HAL_UART_Transmit(&huart2, "Init Lora\n", 10, HAL_MAX_DELAY);

	/* Do until received an OK, after 10 times, send ERROR over Dash7.
	 * Reset the LoRa Module: <ATZ> */
	uint8_t reset_cmd[] = { 0x41, 0x54, 0x5A, 0x0d, 0x0a };
	HAL_UART_Transmit(&huart3, reset_cmd, 5, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, "RESET\n", 6, HAL_MAX_DELAY);

	/* Delay until the module is reset */
	HAL_Delay(2000);

	/* Sent message has not to be confirmed */
	uint8_t confirm_cmd[] = { 0x41, 0x54, 0x2b, 0x43, 0x46, 0x4d, 0x3d, 0x31,
			0x0d, 0x0a };
	HAL_UART_Transmit(&huart3, confirm_cmd, 10, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, "CONFIRM MODE\n", 13, HAL_MAX_DELAY);
	HAL_Delay(2000);

	/* Flush the UART register */
	__HAL_UART_FLUSH_DRREGISTER(&huart3);

	/* AT command to join a network: <AT+JOIN> */
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

/**
 * @brief Check whether LORA has joined the network.
 */
void checkNetwork() {
	/* Delay to make sure the end node had enough time to join */
	uint8_t at_joined[1];
	int check_joined = 0;
	bool joined_lora;

	HAL_Delay(8000);

	while (check_joined < 50) {
		/* Check if joined: <AT+NJS=?> */
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

/**
 * @brief Sends an error message over DASH7 to the backend
 */
void loraError() {
	HAL_UART_Transmit(&huart2, "JOIN ERROR\n", 11, HAL_MAX_DELAY);
	uint8_t join_error[1] = { 0x45 };

	DASH7Message(join_error, 1);
}

/**
 * @brief initialization of the GPS module
 */
void initGPS() {
	uint8_t gga_off[] = { 0x24, 0x50, 0x53, 0x52, 0x46, 0x31, 0x30, 0x33, 0x2c,
			0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x30, 0x2c, 0x30, 0x31,
			0x2a, 0x32, 0x34, 0x0d, 0x0a };
	HAL_UART_Transmit(&huart5, gga_off, 25, HAL_MAX_DELAY);
	/* NMAE cmd to receive every minute */
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

/**
 * @brief activating buzzer
 */
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

/**
 * @brief Performs a reset of the WWDG
 */
void resetWWDG(void) {
	HAL_WWDG_Refresh(&WwdgHandle);
}

/**
 * @brief Sends the GPS coordinates over LORA
 */
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
			/* check for the end of the GLL sentence and check if it's a GLL sentence */
			if (*character == '\n') {

				/* AT COMMAND: AT+SEND=2:<gps data><CR><LF> */
				uint8_t AT_COMMAND[] = { 0x41, 0x54, 0x2b, 0x53, 0x45, 0x4e,
						0x44, 0x3d, 0x32, 0x3a };
				for (int j = 0; j <= i; j++) {
					AT_COMMAND[10 + j] = buffer[j];
				}

				/* Add <CR><LF> */
				AT_COMMAND[i + 11] = 0x0d;
				AT_COMMAND[i + 12] = 0x0a;

				/* Send over LoRa */
				HAL_UART_Transmit(&huart2, AT_COMMAND, i + 10, HAL_MAX_DELAY);
				HAL_UART_Transmit(&huart3, AT_COMMAND, i + 10, HAL_MAX_DELAY);
				i = 0;
				BuzzerAlert();
			}

			i = i + 1;

		}
	}
	/* Toggle pin PC9 */
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);

	return;
}

/**
 * @brief Function to send a DASH7 message with a variable length:
 * - 0x41 0x54 0x24 0x44 0xc0 0x00 0x0d
 * - 0xb4 0x13 0x32 0xd7 0x00 0x00 0x10 0x01 0x20 0x40
 * - 0x00 0x01 0x01
 * @param data the data to be sends.
 * @param lengthDash7 the length of the data to be send.
 */
void DASH7Message(uint8_t data[], int lengthDash7) {
	int total_length = CMD_LENGTH - 7 + lengthDash7;
	uint8_t CMD[] = { 0x41, 0x54, 0x24, 0x44, 0xc0, 0x00, total_length, 0xb4,
			0x13, 0x32, 0xd7, 0x00, 0x00, 0x10, 0x01, 0x20, 0x40, 0x00,
			lengthDash7, };

	/* Create the ALP array with the right length */
	uint8_t ALP[total_length + 7];

	for (int i = 0; i <= CMD_LENGTH; i++) {
		ALP[i] = CMD[i];
	}

	/* Put the data in de ALP command */
	for (int k = 0; k <= lengthDash7; k++) {
		/* ALP length is 16 and + 1 for lengthDash7 field */
		ALP[CMD_LENGTH + k] = data[k];
	}

	HAL_UART_Transmit(&huart4, ALP, sizeof(ALP), HAL_MAX_DELAY);

}

/**
 * @brief Reads the output registers of the LPS22HB sensor and sends them over DASH7.
 */
void getAndSendAltitude() {

	/* Set one shot mode for barometer for low power consumption */
	Settings = LPS22HB_ONE__SHOT_ENABLE;
	HAL_I2C_Mem_Write(&hi2c1, LPS22HB_BARO_ADDRESS, LPS22HB_CTRL_REG2, 1,
			&Settings, 1, 100);
	/* Read the output registers of the LPS22HB */
	HAL_I2C_Mem_Read(&hi2c1, LPS22HB_BARO_ADDRESS, LPS22HB_PRESS_OUT_XL, 1,
			&LSBpressure, 1, 100);
	HAL_I2C_Mem_Read(&hi2c1, LPS22HB_BARO_ADDRESS, LPS22HB_OUT_L, 1,
			&MIDpressure, 1, 100);
	HAL_I2C_Mem_Read(&hi2c1, LPS22HB_BARO_ADDRESS, LPS22HB_OUT_H, 1,
			&MSBpressure, 1, 100);
	HAL_I2C_Mem_Read(&hi2c1, LPS22HB_BARO_ADDRESS, LPS22HB_TEMP_OUT_L, 1,
			&LSBtemp, 1, 100);
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

///**
// * @brief Read a certain register of the LSM303AGR depending on the value of ACC_MAG:
// * 		- if this value is 0: read out a register of the accelerometer
// * 		- if this value is 1: read out a register of the magnetormeter
// * The standard HAL library values are used for the size of the register and time-out.
// * @param LSM303AGR_reg: the register that needs to be read of the LSM303AGR
// * @param LSM303AGR_data: a pointer where the data of the register needs to be stored
// * @param ACC_MAG: depending on the value a value a register of the accelerometer of magnetometer are read
// * @return returns the status of the I2C transfer
// */
//void LSM303AGR_readRegister(uint8_t LSM303AGR_reg, uint8_t LSM303AGR_data,
//		uint8_t ACC_MAG) {
//
//	if (ACC_MAG == 0) {
//		HAL_I2C_Mem_Read(&hi2c1, LSM303AGR_ACC_I2C_ADDRESS, LSM303AGR_reg,
//		I2C_MEMADD_SIZE_8BIT, &LSM303AGR_data, sizeof(LSM303AGR_data),
//		HAL_MAX_DELAY);
//	} else {
//		HAL_I2C_Mem_Read(&hi2c1, LSM303AGR_MAG_I2C_ADDRESS, LSM303AGR_reg,
//		I2C_MEMADD_SIZE_8BIT, &LSM303AGR_data, sizeof(LSM303AGR_data),
//		HAL_MAX_DELAY);
//	}
//
//}
//
///**
// * @brief Writes a cerain value to a certain register of the LSM303AGR depending on the value of ACC_MAG:
// *   	- if this value is 0: write to a register of the accelerometer
// * 		- if this value is 1: write to a register of the magnetormeter
// * the standard HAL library values are used for the size of the register and time-out.
// * @param LSM303AGR_reg: the register where data needs to be written to
// * @param LSM303AGR_data: a pointer to the data that needs to be written
// * @param ACC_MAG: depending on the value, a write is performed to the accelerometer of magnetometer
// * @return returns the status of the I2C transfer
// */
//void LSM303AGR_writeRegister(uint8_t LSM303AGR_reg, uint8_t LSM303AGR_data,
//		uint8_t ACC_MAG) {
//
//	if (ACC_MAG == 0) {
//		HAL_I2C_Mem_Write(&hi2c1, LSM303AGR_ACC_I2C_ADDRESS, LSM303AGR_reg,
//		I2C_MEMADD_SIZE_8BIT, &LSM303AGR_data, sizeof(LSM303AGR_data),
//		HAL_MAX_DELAY);
//	} else {
//		HAL_I2C_Mem_Write(&hi2c1, LSM303AGR_MAG_I2C_ADDRESS, LSM303AGR_reg,
//		I2C_MEMADD_SIZE_8BIT, &LSM303AGR_data, sizeof(LSM303AGR_setting),
//		HAL_MAX_DELAY);
//	}
//
//}
//
///**
// * @brief initialization of the LSM303AGR
// */
//void LSM303AGR_init() {
//
//	LSM303AGR_setting = LSM303AGR_ACC_BOOT;
//	LSM303AGR_writeRegister(LSM303AGR_ACC_CTRL_REG5, LSM303AGR_setting, 0);
//
//	LSM303AGR_setting = LSM303AGR_MAG_SOFT_RST;
//	LSM303AGR_writeRegister(LSM303AGR_MAG_CFG_REG_A, LSM303AGR_setting, 1);
//
//	LSM303AGR_setting = LSM303AGR_ACC_ODR_1HZ | LSM303AGR_ACC_X_EN
//			| LSM303AGR_ACC_Y_EN | LSM303AGR_ACC_Z_EN;
//	LSM303AGR_writeRegister(LSM303AGR_ACC_CTRL_REG1, LSM303AGR_setting, 0);
//
//	LSM303AGR_setting = LSM303AGR_ACC_BDU_EN;
//	LSM303AGR_writeRegister(LSM303AGR_ACC_CTRL_REG4, LSM303AGR_setting, 0);
//
//	LSM303AGR_setting = LSM303AGR_MAG_COMP_TEMP_EN | LSM303AGR_MAG_LP_EN
//			| LSM303AGR_MAG_ODR_10HZ;
//	LSM303AGR_writeRegister(LSM303AGR_MAG_CFG_REG_A, LSM303AGR_setting, 1);
//
//	LSM303AGR_setting = LSM303AGR_MAG_OFF_CANC | LSM303AGR_MAG_LPF;
//	LSM303AGR_writeRegister(LSM303AGR_MAG_CFG_REG_B, LSM303AGR_setting, 1);
//
//	LSM303AGR_setting = LSM303AGR_MAG_BDU;
//	LSM303AGR_writeRegister(LSM303AGR_MAG_CFG_REG_C, LSM303AGR_setting, 1);
//
//}
//
///**
// * @brief Reads the output registers of the accelerometer
// * @param pData: a pointer to where the data needs to be stored
// */
//void LSM303AGR_ACC_readAccelerationData(int32_t *pData) {
//
//	LSM303AGR_ACC_TEMP_DATA rawData;
//
//	HAL_I2C_Mem_Read(&hi2c1, LSM303AGR_ACC_I2C_ADDRESS,
//			LSM303AGR_ACC_MULTI_READ, I2C_MEMADD_SIZE_8BIT,
//			rawData.registerData, sizeof(rawData.registerData), HAL_MAX_DELAY);
//
//	rawData.rawData[0] = (rawData.registerData[1] << 8)
//			| rawData.registerData[0];
//	rawData.rawData[1] = (rawData.registerData[3] << 8)
//			| rawData.registerData[2];
//	rawData.rawData[2] = (rawData.registerData[5] << 8)
//			| rawData.registerData[4];
//
//	/* Apply proper shift and sensitivity */
//	// Normal mode 10-bit, shift = 6 and FS = 2
//	pData[0] = (int32_t) (((rawData.rawData[0] >> 6) * 3900 + 500) / 1000);
//	pData[1] = (int32_t) (((rawData.rawData[1] >> 6) * 3900 + 500) / 1000);
//	pData[2] = (int32_t) (((rawData.rawData[2] >> 6) * 3900 + 500) / 1000);
//
//}
//
///**
// * @brief Reads the output registers of the magnetometer and applies the sensitivity
// * @param pData: a pointer to where the data needs to be stored
// */
//void LSM303AGR_MAG_readMagneticData(int32_t *pData) {
//
//	LSM303AGR_MAG_TEMP_DATA rawData;
//
//	HAL_I2C_Mem_Read(&hi2c1, LSM303AGR_MAG_I2C_ADDRESS,
//			LSM303AGR_MAG_MULTI_READ, I2C_MEMADD_SIZE_8BIT,
//			rawData.registerData, sizeof(rawData.registerData), HAL_MAX_DELAY);
//
//	rawData.rawData[0] = (rawData.registerData[1] << 8)
//			| rawData.registerData[0];
//	rawData.rawData[1] = (rawData.registerData[3] << 8)
//			| rawData.registerData[2];
//	rawData.rawData[2] = (rawData.registerData[5] << 8)
//			| rawData.registerData[4];
//
//	/* Calculate the data. */
//	pData[0] = (int32_t) (rawData.rawData[0] * 1.5f);
//	pData[1] = (int32_t) (rawData.rawData[1] * 1.5f);
//	pData[2] = (int32_t) (rawData.rawData[2] * 1.5f);
//
//}
//
///**
// * @brief The ecompass algorithm, it does the following:
// * 		1. Read the data from the accelerometer and magnetomerer
// * 		2. Convert the data from integer to float
// * 		3. Apply the resolution on the accelerometer data
// * 		4. Calculate the phi (roll angle) value
// * 		5. Calculate the  theta (pitch angle or attitude) value
// * 		6. Calculate of the Psi (yaw angle or heading) value
// *
// * @return the value of the heading in uint16_t format.
// */
//void ecompassAlgorithm() {
//
//	LSM303AGR_ACC_readAccelerationData(lsm303agrAccData);
//	LSM303AGR_MAG_readMagneticData(lsm303agrMagData);
//
////Casting from integer to float (6 castings necessary)
//	lsm303agrAccDataFloat[0] = lsm303agrAccData[0];
//	lsm303agrAccDataFloat[1] = lsm303agrAccData[1];
//	lsm303agrAccDataFloat[2] = lsm303agrAccData[2];
//
//	lsm303agrMagDataFloat[0] = lsm303agrMagData[0];
//	lsm303agrMagDataFloat[1] = lsm303agrMagData[1];
//	lsm303agrMagDataFloat[2] = lsm303agrMagData[2];
//
//	lsm303agrAccDataFloat[0] = (lsm303agrAccDataFloat[0] * 2)
//			/ pow(2, (resolution - 1));
//	lsm303agrAccDataFloat[1] = (lsm303agrAccDataFloat[1] * 2)
//			/ pow(2, (resolution - 1));
//	lsm303agrAccDataFloat[2] = (lsm303agrAccDataFloat[2] * 2)
//			/ pow(2, (resolution - 1));
//
//	//Computation of Phi (roll angle) in radians and degrees
//	rollRad = atan2f(lsm303agrAccDataFloat[1], lsm303agrAccDataFloat[2]);
//	rollDegree = rollRad * (180 / PI);
//
//	//Computation of Theta (pitch angle or attitude) in radians and degrees
//	lsm303agrAccDataTemp = lsm303agrAccData[1] * sinf(rollRad)
//			+ lsm303agrAccData[2] * cosf(rollRad);
//	pitchRad = atan2f(-lsm303agrAccData[0], lsm303agrAccDataTemp);
//	pitchDegree = pitchRad * (180 / PI);
//
//	//Computation of Psi (yaw angle, or heading)
//	yawTemp[0] = lsm303agrMagDataFloat[2] * sinf(rollRad)
//			- lsm303agrMagDataFloat[1] * cosf(rollRad);
//	yawTemp[1] = lsm303agrMagDataFloat[1] * sinf(rollRad)
//			+ lsm303agrMagDataFloat[2] * cosf(rollRad);
//	yawTemp[2] = lsm303agrMagDataFloat[0] * cosf(pitchRad)
//			+ yawTemp[1] * sinf(pitchRad);
//	yawRad = atan2f(yawTemp[0], yawTemp[2]);
//	yawDegree = yawRad * (180 / PI);
//
//	if (yawDegree < 0)
//		yawDegree += 360;
//
//	dir = (uint16_t) yawDegree;
//
//	uint8_t highPart = (uint8_t) (dir >> 8);
//	uint8_t lowPart = (uint8_t) dir;
//
//	uint8_t dataToSend[] = { 0x4D, highPart, lowPart };
//	DASH7Message(dataToSend, 3);
//	HAL_UART_Transmit(&huart2, "Sensor used!\n", sizeof("Sensor used!\n") - 1,
//	HAL_MAX_DELAY);
//
//}

/**
 * @brief System clock configuration
 */
void SystemClock_Config(void) {

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	/* Configure the main internal regulator output voltage */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/*Initializes the CPU, AHB and APB busses clocks */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_5;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/* Initializes the CPU, AHB and APB busses clocks */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/* Configure the Systick interrupt time */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	/*Configure the Systick */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
 * @brief IC21 initialization function
 */
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

/**
 * brief USART4 initialization function
 */
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

/**
 * brief USART5 initialization function, this USART is used by the GPS module
 */
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

/**
 * @brief USART2 initialization function, this USART is used for debugging
 */
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

/**
 * @brief USART3 initialization function
 */
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

/**
 * @brief TIMER2 initialization function
 */
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

/** @brief Configure the GPIO pins as follow:
 * - Analog
 * - Input
 * - Output
 * - EVENT_OUT
 * - EXTI
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

//	/*Configure GPIO pins : PC14 PC15 PC0 PC1
//		 PC2 PC3 PC4 PC5
//		 PC6 PC8 PC9 */
//		GPIO_InitStruct.Pin = GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_0 | GPIO_PIN_1
//				| GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6
//				| GPIO_PIN_8 | GPIO_PIN_9;
//		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//		GPIO_InitStruct.Pull = GPIO_NOPULL;
//		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
//
//		/*Configure GPIO pins : PH0 PH1 */
//		GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
//		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//		GPIO_InitStruct.Pull = GPIO_NOPULL;
//		HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
//
//		/*Configure GPIO pins : PA0 PA1 PA4
//		 PA6 PA7 PA8 PA9
//		 PA10 PA11 PA12 PA13
//		 PA14 PA15 */
//		GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_6
//				| GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11
//				| GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
//		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//		GPIO_InitStruct.Pull = GPIO_NOPULL;
//		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//		/*Configure GPIO pins : PB0 PB1 PB2 PB12
//		 PB13 PB14 PB15 PB4
//		 PB5 PB7 */
//		GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_12
//				| GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_4 | GPIO_PIN_5
//				| GPIO_PIN_7;
//		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//		GPIO_InitStruct.Pull = GPIO_NOPULL;
//		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/**
 * @brief  This function is executed in case of error occurrence.
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

/* *********************** (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
