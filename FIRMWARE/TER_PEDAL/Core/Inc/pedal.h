/*
 * pedal.h
 *
 *  Created on: Feb 6, 2024
 *      Author: Ozuba
 */

#ifndef INC_PEDAL_H_
#define INC_PEDAL_H_

#include <stdlib.h>
#include "stm32f1xx_hal.h"
#include "ee.h"  //Librería para emulación eeprom, guardar valores de calibracion en la flash
#include "adc.h" //para realizar lecturas
#include "TeR_UTILS.h"
#include "TeR_CAN.h"

#define MAXWHEELANGLE 30 //Angulo limite de la rueda
#define MARGIN 200 //Points of adc

//Brake min and max calculated with a 5v supply
#define MINBRAKE VOLT2ADC(0.5,5.0)
#define MAXBRAKE VOLT2ADC(4.5,5.0)

struct offsets_t {
	uint32_t low[3];
	uint32_t high[3];
	uint8_t written; //esta variable permite programar placas rápido ya que esta a 0 únicamente si la placa
};
//Para que los comandos puedan cambiarla (Abstraer a futuro)
extern struct offsets_t offset;
extern int32_t adcReadings[4];


void initPedal(ADC_HandleTypeDef* hadc); //Carga los offsets y lanza la interrupcion del adc
void readSensors();



#endif /* INC_PEDAL_H_ */
