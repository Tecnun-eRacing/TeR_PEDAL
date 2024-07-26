/*
 * TeR_CAN.c
 *
 *  Created on: Feb 2, 2024
 *      Author: Ozuba
 *
 * ████████╗███████╗██████╗          ██████╗ █████╗ ███╗   ██╗
 * ╚══██╔══╝██╔════╝██╔══██╗        ██╔════╝██╔══██╗████╗  ██║
 *    ██║   █████╗  ██████╔╝        ██║     ███████║██╔██╗ ██║
 *    ██║   ██╔══╝  ██╔══██╗        ██║     ██╔══██║██║╚██╗██║
 *    ██║   ███████╗██║  ██║███████╗╚██████╗██║  ██║██║ ╚████║
 *    ╚═╝   ╚══════╝╚═╝  ╚═╝╚══════╝ ╚═════╝╚═╝  ╚═╝╚═╝  ╚═══╝
 */

/*
 *  Este Fichero tiene como Objetivo almacenar las funciones de decodificación
 *  y envío de todos los mensajes de una placa, incluye como librerías aquellas
 *  autogeneradas mediante cantools y ofrece una interfáz de cara al micro con dos
 *  Funciones:
 *   - initCAN -> arranca el can asignandolo a un timer para la periodicidad del envio
 *  - decodeMSG -> Decodifica las estructuras pertinentes
 *  - sendCAN -> Envía los mensajes pertinentes
 *  - command -> Función que se llama cuando se recibe el mensaje de comando para que cada placa lo interprete como corresponde
 *  A su vez están creados aqui todas las estructuras de memoria del can
 *
 */

#include "TeR_CAN.h"

/* ---------------------------[Estructuras del CAN]-------------------------- */
//Pointer to timer and can peripheral being used
CAN_HandleTypeDef *can;
TIM_HandleTypeDef *tim;

//Datos transmision
CAN_TxHeaderTypeDef TxHeader; //Header de transmisión
uint8_t TxData[8]; //Header de recepción
uint32_t TxMailbox; //Mailbox para el CAN1

//Datos recepcion
CAN_RxHeaderTypeDef RxHeader;
uint8_t RxData[8];

//Variable msgIndex para la cola de envio
uint8_t msgIndex = 0; //Hasta 255 mensajes
/* -------------------------------------------------------------------------- */

struct TeR_t TeR;

uint8_t initCAN(CAN_HandleTypeDef *hcan, TIM_HandleTypeDef *htim) {
	//Inicializacion del periferico CAN
	can = hcan;
	tim = htim;
	//Arranque del periferico y la interrupcion
	HAL_CAN_Start(can); //Activamos el can
	HAL_CAN_ActivateNotification(can, CAN_IT_RX_FIFO0_MSG_PENDING); //Activamos notificación de mensaje pendiente a lectura
	HAL_TIM_Base_Start_IT(tim); //Arranca el envio temporizado
	return 1;
}

//Envío temporizado
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) { //Envio temporizado
	if (htim == tim) { //Si es nuestro timer(Da igual si solo hay 1)
		readSensors(); //Lee los sensores
		sendCAN();
	}
}

//Recepción asincrona
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) { //No hay distinción de bus
	if (hcan == can) { //Si es nuestro can (Da igual si solo hay 1)
		HAL_CAN_GetRxMessage(can, CAN_RX_FIFO0, &RxHeader, RxData); //Recoge el mensaje
		decodeMsg(RxHeader.StdId, RxData); //llama a la decodificación
	}
}
/* -------------------------------------------------------------------------- */

//Función de decodificación del CAN, si quieres que la ecu disponga de una señal hay que añadirla aquí.
uint8_t decodeMsg(uint32_t canId, uint8_t *data) {

	switch (canId) {
	//Attend the command
	case TER_COMMAND_FRAME_ID:
		command(data[0]); //Llama a la interpretación del comando
		break;

	default:
		return -1;
		break;

	}
	return 1;
}

//Función de envío de mensajes
uint8_t sendCAN(void) {
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;
	if (HAL_CAN_GetTxMailboxesFreeLevel(can) > 0) { // Hay un slot para nuestro mensaje
		switch (msgIndex++) {

		case 0: //APPS
			TxHeader.StdId = TER_APPS_FRAME_ID;
			TxHeader.DLC = TER_APPS_LENGTH;
			ter_apps_pack(TxData, &TeR.apps, sizeof(TxData)); //Empaquetamos
			break;
		case 1: //BPPS
			TxHeader.StdId = TER_BPPS_FRAME_ID;
			TxHeader.DLC = TER_BPPS_LENGTH;
			ter_bpps_pack(TxData, &TeR.bpps, sizeof(TxData)); //Empaquetamos
			break;

		case 2: //STEER
			TxHeader.StdId = TER_STEER_FRAME_ID;
			TxHeader.DLC = TER_STEER_LENGTH;
			ter_steer_pack(TxData, &TeR.steer, sizeof(TxData)); //Empaquetamos
			break;

		default: //Esto evita tener que contar mensajes
			msgIndex = 0; //cualquier otro valor retorna al ultimo mensaje
			return 1; //Evita que se envíe un mensaje doble terminando la funcion
			break;
		}
		if (HAL_CAN_AddTxMessage(can, &TxHeader, TxData, &TxMailbox)
				!= HAL_OK) {
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1); //Indicate Error with light
		}
	}
	return 1;
}

//Implementa aqui los comandos que se han de ejecutar
uint8_t command(uint8_t cmd) {
	switch (cmd) {
	case TER_COMMAND_CMD_CALIBRATE_APPS_MIN_CHOICE: //Calibrate ACC 0% Pos and Store
		offset.low[2] = adcReadings[2]; //Recoje el valor actual
		offset.low[1] = adcReadings[1];
		ee_writeToRam(0, sizeof(offset), (uint8_t*) &offset); //Almacena
		ee_commit();
		break;

	case TER_COMMAND_CMD_CALIBRATE_APPS_MAX_CHOICE: //Calibrate ACC 100% Pos and Store
		offset.high[2] = adcReadings[2]; //Recoje el valor actual
		offset.high[1] = adcReadings[1];
		ee_writeToRam(0, sizeof(offset), (uint8_t*) &offset); //Almacena
		break;

	case TER_COMMAND_CMD_CALIBRATE_STEER_RIGHTEST_CHOICE: //Calibrate Rightest Steer Position
		offset.low[0] = adcReadings[0]; //Recoje el valor actual
		ee_writeToRam(0, sizeof(offset), (uint8_t*) &offset); //Almacena
		break;

	case TER_COMMAND_CMD_CALIBRATE_STEER_LEFTEST_CHOICE: //Calibrate Leftest Steer Position
		offset.high[0] = adcReadings[0]; //Recoje el valor actual
		ee_writeToRam(0, sizeof(offset), (uint8_t*) &offset); //Almacena
		break;

	default:
		return 0;
		break;

	}
	ee_commit(); //Almacena en la flash la calibración
	return 1;
}
