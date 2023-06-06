/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include "ter.h" // Master CAN DBC
#include "ee.h"  //Librería para emulación eeprom, guardar valores de calibracion en la flash
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//Datos transmision
CAN_TxHeaderTypeDef TxHeader; //Header de transmisión
uint8_t TxData[8]; //Header de recepción
uint32_t TxMailbox; //Mailbox para el periferico
//Datos recepcion
CAN_RxHeaderTypeDef RxHeader;
uint8_t RxData[8];

//Implausabilities
uint32_t imp_timestamp;
bool apps_imp = 0; //Estado del implausability

//Offsets de los sensores {Sens1,Sens2}
struct offsets_t {
	uint32_t low[2];
	uint32_t high[2];
} offset, test;
//Mensajes
struct ter_apps_t apps; //Aceleradores
struct ter_bpps_t bpps; //Freno

//Estructura de lectura para el ADC
uint32_t adcReadings[3]; //32*3, el adc saca 12 bits alineados a la derecha

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
void command(uint8_t cmd, uint8_t *args); //Gestiona los comandos recibidos
void readSensors(void); //Lectura de todos los sensores
uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min,uint32_t out_max); //Mapea un intervalo sobre otro (Cogida de Arduino)
void sendCan(void); //Envio de todos los mensajes







/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	ee_init(); //Inicializamos la flash (EEPROM virtual)
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_CAN_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
	//Inicializamos el DMA para que copie nuestros datos al buffer de lecturas
	//Hemos desactivado las interrupciones del mismo en el NVIC para que no obstruyan, solo nos interesa que anden disponibles
	HAL_ADC_Start_DMA(&hadc1, adcReadings, 3); // Arrancamos el ADC en modo DMA

	//Inicializacion del periferico CAN
	HAL_CAN_Start(&hcan); //Activamos el can
	HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING); //Activamos notificación de mensaje pendiente a lectura



	//Carga de los offsets
	ee_read(0, sizeof(offset), (uint8_t*) &offset); //Lee de memoria el struct
	/*Para Programar si se borra la flash
	 offset.high[0] = 4096;
	 offset.high[1] = 4096;
	 offset.low[0] = 0;
	 offset.low[1] = 0;
	 ee_writeToRam(0, sizeof(offset), &offset);
	 ee_commit();
	 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		//Zona Lectura de Sensores
		readSensors();
		//envio del CAN
		sendCan();







		HAL_Delay(100);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* USB_LP_CAN1_RX0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
}

/* USER CODE BEGIN 4 */
void readSensors() {
	//Se leen y convierten las señales
	apps.apps_1 = map(adcReadings[0], offset.low[0], offset.high[0], 0, 255); //Lectura del ADC 1
	apps.apps_2 = map(adcReadings[1], offset.low[1], offset.high[1], 0, 255); //Lectura del ADC 2
	//Check for implausability
	if (abs(apps.apps_1 - apps.apps_2) > 255 * 10 / 100) {//T 11.8.9 Desviacion de 10 puntos en %
		if (imp_timestamp == 0) {	 //Si no había timestamp activalo
			imp_timestamp = HAL_GetTick();
		} else if (HAL_GetTick() - imp_timestamp > 100) {//Si el tiempo es mayor que 100 millis
			apps.imp_flags = 1; //Activa el implausability y dejalo latched
			imp_timestamp = 0; //Resetea el counter
		}
	} else { //Si vuelve a estar bien desactiva el contador
		imp_timestamp = 0;
	}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, apps.imp_flags); //Actualizamos el estado del led
}



void command(uint8_t cmd, uint8_t *args) {
	switch (cmd) {
	case 1: //Calibrate ACC 0% Pos and Store
		offset.low[0] = adcReadings[0]; //Recoje el valor actual
		offset.low[1] = adcReadings[1];
		ee_writeToRam(0, sizeof(offset), (uint8_t*) &offset); //Almacena
		break;

	case 2: //Calibrate ACC 100% Pos and Store
		offset.high[0] = adcReadings[0]; //Recoje el valor actual
		offset.high[1] = adcReadings[1];
		ee_writeToRam(0, sizeof(offset), (uint8_t*) &offset); //Almacena
		break;

	}

}

void sendCan(){
		TxHeader.IDE = CAN_ID_STD;
		TxHeader.StdId = TER_APPS_FRAME_ID;
		TxHeader.RTR = CAN_RTR_DATA;
		TxHeader.DLC = TER_APPS_LENGTH;
		ter_apps_pack(TxData, &apps, sizeof(TxData));//Empaquetamos
		if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK) {
		Error_Handler();
	}



}

uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min,
		uint32_t out_max) {
	long val = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	return (val > 0) ? val : 0; //Trukelele mirate lo q es un ternary operator man (Satura por debajo de 0)
}

///////////////////////////////////////////////////////////////[Interrupciones]////////////////////////////////////////////////////////////////////////////////
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData); //Recoge el mensaje
	if (RxHeader.StdId == TER_PEDAL_CMD_FRAME_ID) { //Issued Command
		command(RxData[0], &RxData[1]); //Envia comando y argumentos
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
