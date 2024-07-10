/*
 * TeR_CAN.h
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


/*  Este Fichero tiene como Objetivo almacenar las funciones de decodificación
 *  y envío de todos los mensajes de una placa, incluye como librerías aquellas
 *  autogeneradas mediante cantools y ofrece una interfáz de cara al micro con dos
 *  Funciones:
 *  - initCAN -> arranca el can asignandolo a un timer para la periodicidad del envio
 *  - decodeMSG -> Decodifica las estructuras pertinentes
 *  - sendCAN -> Envía los mensajes pertinentes (Esto no va a depender del estado, ya que los inverters siempre estarán a 0)
 *	- cmd() -> Función que se llama cuando se recibe el mensaje de comando para que cada placa lo interprete como corresponde
 *
 *  A su vez están creados aqui todas las estructuras de memoria del CAN que permiten su uso fuera de el
 */




#ifndef INC_TER_CAN_H_
#define INC_TER_CAN_H_

#include "ter.h"
#include "stm32f1xx_hal.h"
#include "pedal.h"
#include "can.h"
/* --------------------- Estructuras de datos del coche ----------------- */
//TER.dbc
struct TeR_t{
	//Propias
	struct ter_apps_t apps; //Aceleradores
	struct ter_bpps_t bpps; //Freno
	struct ter_steer_t steer; //Volante
};
extern struct TeR_t TeR; //Expone los datos del TeR a otros archivos
/* ---------------------------------------------------------------------- */



uint8_t initCAN(CAN_HandleTypeDef *hcan, TIM_HandleTypeDef *htim);//Initializes CAN
uint8_t decodeMsg(uint32_t canId, uint8_t *data); //Decodes message according to DBC
uint8_t sendCAN(void);
uint8_t command(uint8_t cmd); //

#endif /* INC_TER_CAN_H_ */
