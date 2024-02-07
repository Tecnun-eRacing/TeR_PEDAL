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
#include "TeR_CAN.h"

#define MAXWHEELANGLE 30 //Angulo limite de la rueda

struct offsets_t {
	uint32_t low[4];
	uint32_t high[4];
	uint8_t written; //esta variable permite programar placas rápido ya que esta a 0 únicamente si la placa
};
//Para que los comandos puedan cambiarla (Abstraer a futuro)
extern struct offsets_t offset;
extern uint32_t adcReadings[4];


void initPedal(ADC_HandleTypeDef* hadc); //Carga los offsets y lanza la interrupcion del adc
void readSensors();
int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min,
		int32_t out_max); //Mapea un intervalo sobre otro (Cogida de Arduino)



#endif /* INC_PEDAL_H_ */
