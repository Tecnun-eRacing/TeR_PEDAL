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
 *  - decodeMSG -> Decodifica las estructuras pertinentes
 *  - sendCAN -> Envía los mensajes pertinentes (Esto no va a depender del estado, ya que los inverters siempre estarán a 0)
 *  - command -> Función que se llama cuando se recibe el mensaje de comando para que cada placa lo interprete como corresponde
 *  A su vez están creados aqui todas las estructuras de memoria del can
 *
 */

#include "TeR_CAN.h"

/* ---------------------------[Estructuras del CAN]-------------------------- */
//Datos transmision
CAN_TxHeaderTypeDef TxHeader; //Header de transmisión
uint8_t TxData[8]; //Header de recepción
uint32_t TxMailbox; //Mailbox para el CAN1

//Datos recepcion
CAN_RxHeaderTypeDef RxHeader;
uint8_t RxData[8];
/* -------------------------------------------------------------------------- */

struct TeR_t TeR;

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) { //No hay distinción de bus
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData); //Recoge el mensaje
	decodeMsg(RxHeader.StdId, RxData); //llama a la decodificación
}

//Función de decodificación del CAN, si quieres que la ecu disponga de una señal hay que añadirla aquí.
uint8_t decodeMsg(uint32_t canId, uint8_t *data) {

	switch (canId) {
	//Attend the command
	case TER_CMD_FRAME_ID:
		command(data[0], &data[1]); //Llama a la interpretación del comando
		break;

	default:
		return -1;
		break;

	}
	return 1;
}

//Función de envío de mensajes
uint8_t sendCAN(void) {
	//APPS
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.StdId = TER_APPS_FRAME_ID;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = TER_APPS_LENGTH;
	ter_apps_pack(TxData, &TeR.apps, sizeof(TxData)); //Empaquetamos
	if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1); //Indicate Error with light
	}
	//BPPS
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.StdId = TER_BPPS_FRAME_ID;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = TER_BPPS_LENGTH;
	ter_bpps_pack(TxData, &TeR.bpps, sizeof(TxData)); //Empaquetamos
	if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1); //Indicate Error with light
	}
	//STEER
	TxHeader.IDE = CAN_ID_STD;
	TxHeader.StdId = TER_STEER_FRAME_ID;
	TxHeader.RTR = CAN_RTR_DATA;
	TxHeader.DLC = TER_STEER_LENGTH;
	ter_steer_pack(TxData, &TeR.steer, sizeof(TxData)); //Empaquetamos
	if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 1); //Indicate Error with light
	}
	return 1;
}

//Implementa aqui los comandos que se han de ejecutar
uint8_t command(uint8_t cmd, uint8_t *args) {
	switch (cmd) {
	case 1: //Calibrate ACC 0% Pos and Store
		offset.low[2] = adcReadings[2]; //Recoje el valor actual
		offset.low[1] = adcReadings[1];
		ee_writeToRam(0, sizeof(offset), (uint8_t*) &offset); //Almacena
		ee_commit();
		break;

	case 2: //Calibrate ACC 100% Pos and Store
		offset.high[2] = adcReadings[2]; //Recoje el valor actual
		offset.high[1] = adcReadings[1];
		ee_writeToRam(0, sizeof(offset), (uint8_t*) &offset); //Almacena
		break;
	case 3: //Calibrate BPPS 0% Pos
		offset.low[3] = adcReadings[3]; //Recoje el valor actual
		ee_writeToRam(0, sizeof(offset), (uint8_t*) &offset); //Almacena
		break;

	case 4: //Calibrate BPPS 100% Pos
		offset.high[3] = adcReadings[3]; //Recoje el valor actual
		ee_writeToRam(0, sizeof(offset), (uint8_t*) &offset); //Almacena
		break;

	case 5: //Calibrate Rightest Steer Position
		offset.low[0] = adcReadings[0]; //Recoje el valor actual
		ee_writeToRam(0, sizeof(offset), (uint8_t*) &offset); //Almacena
		break;

	case 6: //Calibrate Leftest Steer Position
		offset.high[0] = adcReadings[0]; //Recoje el valor actual
		ee_writeToRam(0, sizeof(offset), (uint8_t*) &offset); //Almacena
		break;

	case 7: //Reset de la implausability
		TeR.apps.imp_flag = 0;
		break;

	}
	ee_commit(); //Almacena en la flash la calibración
	return 1;
}
