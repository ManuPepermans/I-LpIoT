
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE BEGIN Includes */
#include <limits.h>
#include <math.h>
#include <string.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;


//Rejestry
#define LSM303_ACC_ADDRESS (0x19 << 1) // ADRESS from the accelero
#define LSM303_MAG_ADDRESS 0x3C // ADRESS from the magnetometer
#define LPS22HB_BARO_ADDRESS 0xBA // ADRESS from the barometer  "10111010"

#define LPS22HB_AWHO_AM_I 0x0F
#define LPS22HB_CTRL_REG2 0x11
#define LPS22HB_PRESS_OUT_XL 0x28	// Pressure output value (LSB)
#define LPS22HB_OUT_L 0x29			// Pressure output value (mid part)
#define LPS22HB_OUT_H 0x2A			// Pressure output value (MSB)
#define LPS22HB_TEMP_OUT_L 0x2B		// Temperature output value (LSB)
#define LPS22HB_TEMP_OUT_H 0x2C		// Temperature output value (MSB)


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

//Set one shot mode for Barometer

#define LPS22HB_ONE__SHOT_ENABLE 0x11 // 0001 0001

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

#define LSM303_ACC_RESOLUTION 2.0 //

//
uint8_t Data[6]; // Var for acc data

uint8_t LSBpressure;
uint8_t MIDpressure;
uint8_t MSBpressure;
uint8_t MSBtemp;
uint8_t LSBtemp;
uint32_t CompletePressure;
uint16_t temperature;

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

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);


int main(void)
{



  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();



  /* Configure the system clock */
  SystemClock_Config();


  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();

  uint8_t Settings = LSM303_ACC_XYZ_ENABLE | LSM303_ACC_100HZ;

  	// I2C write settings
  	HAL_I2C_Mem_Write(&hi2c1, LSM303_ACC_ADDRESS, LSM303_ACC_CTRL_REG1_A, 1, &Settings, 1, 100);
  	Settings = LSM303_ACC_I1_DRDY2;
  	HAL_I2C_Mem_Write(&hi2c1, LSM303_ACC_ADDRESS, LSM303_ACC_CTRL_REG3_A, 1, &Settings, 1, 100);
  	/* USER CODE END 2 */

  	/* Infinite loop */
  	/* USER CODE BEGIN WHILE */
  	while (1) {
  		// Downloading 6 bytes of data containing accelerations in 3 axes
  		HAL_I2C_Mem_Read(&hi2c1, LSM303_ACC_ADDRESS, LSM303_ACC_X_L_A_MULTI_READ, 1, Data, 6, 100);
  		//HAL_UART_Transmit(&huart2,Data,6,HAL_MAX_DELAY);

		HAL_Delay(500);

		//Read accelerometer WHO_AM_I register and print (test)
		//HAL_I2C_Mem_Read(&hi2c1, LSM303_ACC_ADDRESS, LSM303_ACC_WHO_AM_I, 1, Data, 1, 100);
		//HAL_UART_Transmit(&huart2,Data,1,HAL_MAX_DELAY);

		//Set one shot mode for barometer for low power consumption
		Settings = LPS22HB_ONE__SHOT_ENABLE;
		HAL_I2C_Mem_Write(&hi2c1, LPS22HB_BARO_ADDRESS, LPS22HB_CTRL_REG2, 1, &Settings, 1, 100);

		//Read barometer WHO_AM_I register and print (test)
		//HAL_I2C_Mem_Read(&hi2c1, LPS22HB_BARO_ADDRESS, LPS22HB_AWHO_AM_I, 1, Data, 1, 100);
		//HAL_UART_Transmit(&huart2,Data,1,HAL_MAX_DELAY);

		//Read barometer dataRegister(LSB) and print
		HAL_I2C_Mem_Read(&hi2c1, LPS22HB_BARO_ADDRESS, LPS22HB_PRESS_OUT_XL, 1, &LSBpressure, 1, 100);
		//HAL_UART_Transmit(&huart2,&LSBpressure,1,HAL_MAX_DELAY);

		//Read barometer dataRegister(MID) and print
		HAL_I2C_Mem_Read(&hi2c1, LPS22HB_BARO_ADDRESS, LPS22HB_OUT_L, 1, &MIDpressure, 1, 100);
		//HAL_UART_Transmit(&huart2,&MIDpressure,1,HAL_MAX_DELAY);

		//Read barometer dataRegister(MSB) and print
		HAL_I2C_Mem_Read(&hi2c1, LPS22HB_BARO_ADDRESS, LPS22HB_OUT_H, 1, &MSBpressure, 1, 100);
		//HAL_UART_Transmit(&huart2,&MSBpressure,1,HAL_MAX_DELAY);

		//Read temperature dataRegister(LSB) and print
		HAL_I2C_Mem_Read(&hi2c1, LPS22HB_BARO_ADDRESS, LPS22HB_TEMP_OUT_L, 1, &LSBtemp, 1, 100);
		//HAL_UART_Transmit(&huart2,&LSBtemp,1,HAL_MAX_DELAY);

		//Read temperature dataRegister(MSB) and print
		HAL_I2C_Mem_Read(&hi2c1, LPS22HB_BARO_ADDRESS, LPS22HB_TEMP_OUT_H, 1, &MSBtemp, 1, 100);
		//HAL_UART_Transmit(&huart2,&MSBtemp,1,HAL_MAX_DELAY);

		temperature =  (((uint16_t)MSBtemp << 8) | LSBtemp);
		int temperature2 = ((int) temperature) / 100;

		//Combine all data to get the pressure and convert to hPa
		CompletePressure = ((uint32_t)MSBpressure << 16) | (((uint32_t)MIDpressure << 8) | LSBpressure);
		double pressureHPaD = ((double) CompletePressure) / 4096;
		int pressureHPa = ((int) CompletePressure) / 4096;
		double top = pow((1001.1/pressureHPaD),0.1902);
		double top1 = top - 1;
		double top2 = top1 *(temperature+273.15);
		double top3 = top2/0.0065;
		//double h = ((pow((1001.1/pressureHPa),0.1902)-1)*(temperature+273.15))/0.0065;
		int hInt = (int) top3;



		char str2[50];
		sprintf(str2, "%d", temperature2);
		//HAL_UART_Transmit(&huart2,(uint8_t*)str2,strlen(str2),HAL_MAX_DELAY);

		//char str3[50];
		//sprintf(str3, "%d----", hInt);
		//HAL_UART_Transmit(&huart2,(uint8_t*)str3,strlen(str3),HAL_MAX_DELAY);

		//Print pressure in hPa
		char str[50];
		sprintf(str, "%d", pressureHPa);
		//HAL_UART_Transmit(&huart2,(uint8_t*)str,strlen(str),HAL_MAX_DELAY);

		// Mag = 10 Hz (HR and continuous mode) //This has to be set to single mode for lower power consumption
		Settings = LSM303_SETTINGS1;
		HAL_I2C_Mem_Write(&hi2c1, LSM303_MAG_ADDRESS, LSM303_CFG_REG_A_M, 1, &Settings, 1, 100);

		// Mag data-ready interrupt enable
		Settings = LSM303_SETTINGS2;
		HAL_I2C_Mem_Write(&hi2c1, LSM303_MAG_ADDRESS, LSM303_CFG_REG_C_M, 1, &Settings, 1, 100);

		//Read magnetometer WHO_AM_I and print (test)
		//HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, LSM303_MAG_WHO_AM_I, 1, Data, 1, 100);
		//HAL_UART_Transmit(&huart2,Data,1,HAL_MAX_DELAY);

		//Read magnetometer dataregister(1) and print
		HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, OUTX_L_REG_M, 1, &OUTX_L_REG_M_Data, 1, 100);
		//HAL_UART_Transmit(&huart2,&OUTX_L_REG_M_Data,1,HAL_MAX_DELAY);

		//Read magnetometer dataregister(2) and print
		HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, OUTX_H_REG_M, 1, &OUTX_H_REG_M_Data, 1, 100);
		//HAL_UART_Transmit(&huart2,&OUTX_H_REG_M_Data,1,HAL_MAX_DELAY);

		//Read magnetometer dataregister(3) and print
		HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, OUTY_L_REG_M, 1, &OUTY_L_REG_M_Data, 1, 100);
		//HAL_UART_Transmit(&huart2,&OUTY_L_REG_M_Data,1,HAL_MAX_DELAY);

		//Read magnetometer dataregister(4) and print
		HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, OUTY_H_REG_M, 1, &OUTY_H_REG_M_Data, 1, 100);
		//HAL_UART_Transmit(&huart2,&OUTY_H_REG_M_Data,1,HAL_MAX_DELAY);

		//Read magnetometer dataregister(5) and print
		HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, OUTZ_L_REG_M, 1, &OUTZ_L_REG_M_Data, 1, 100);
		//HAL_UART_Transmit(&huart2,&OUTZ_L_REG_M_Data,1,HAL_MAX_DELAY);

		//Read magnetometer dataregister(6) and print
		HAL_I2C_Mem_Read(&hi2c1, LSM303_MAG_ADDRESS, OUTZ_H_REG_M, 1, &OUTZ_H_REG_M_Data, 1, 100);
		//HAL_UART_Transmit(&huart2,&OUTZ_H_REG_M_Data,1,HAL_MAX_DELAY);

		//Omzetten van hexadecimaal naar decimal two's complement



		MAG_X =  (((uint16_t)OUTX_H_REG_M_Data << 8) | OUTX_L_REG_M_Data);
		MAG_Y =  (((uint16_t)OUTY_H_REG_M_Data << 8) | OUTY_L_REG_M_Data);
		MAG_Z =  (((uint16_t)OUTZ_H_REG_M_Data << 8) | OUTZ_L_REG_M_Data);

		int mask = 0xFF; // 11111111

		int MAG_Xtest = ((int) MAG_X) / 256;
		//char str2[50];
		//sprintf(str2, "%d", MAG_Xtest);
		//HAL_UART_Transmit(&huart2,(uint8_t*)str2,strlen(str2),HAL_MAX_DELAY);

		MAG_X ^= mask;
		MAG_Y ^= mask;
		MAG_Z ^= mask;

		MAG_X = (int) MAG_X + 1;
		MAG_Y = (int) MAG_Y + 1;
		MAG_Z = (int) MAG_Z + 1;

  		Xaxis = ((Data[1] << 8) | Data[0]);
  		Yaxis = ((Data[3] << 8) | Data[2]);
  		Zaxis = ((Data[5] << 8) | Data[4]);

  		/* Apply proper shift and sensitivity */
  		int32_t Xaxis_g = ((Xaxis >> 6) * 3900 + 500) / 1000;
  		int32_t Yaxis_g = ((Yaxis >> 6) * 3900 + 500) / 1000;
  		int32_t Zaxis_g = ((Zaxis >> 6) * 3900 + 500) / 1000;
		//HAL_UART_Transmit(&huart2,Data[1],1,HAL_MAX_DELAY);
		//HAL_UART_Transmit(&huart2,Data[2],1,HAL_MAX_DELAY);

  		//Xaxis_g = ((float) Xaxis * LSM303_ACC_RESOLUTION) / (float) INT16_MAX;
  		//Yaxis_g = ((float) Yaxis * LSM303_ACC_RESOLUTION) / (float) INT16_MAX;
  		//Zaxis_g = ((float) Zaxis * LSM303_ACC_RESOLUTION) / (float) INT16_MAX;

  		int Xaxis_gInteger = (int) Xaxis_g;

		char str3[50];
		sprintf(str3, "%d  ", Xaxis_g);
		HAL_UART_Transmit(&huart2,(uint8_t*)str3,strlen(str3),HAL_MAX_DELAY);

		char str5[50];
		sprintf(str5, "%d  ", Yaxis_g);
		HAL_UART_Transmit(&huart2,(uint8_t*)str5,strlen(str5),HAL_MAX_DELAY);

		char str6[50];
		sprintf(str6, "%d  ", Zaxis_g);
		HAL_UART_Transmit(&huart2,(uint8_t*)str6,strlen(str6),HAL_MAX_DELAY);

  		HAL_Delay(3000);
  	}
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

/** Pinout Configuration
*/
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

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
