/*
 * pedal.c
 *
 *  Created on: Feb 6, 2024
 *      Author: Ozuba
 */
#include "pedal.h"


//ADC usado
ADC_HandleTypeDef *adc;
//Implausabilities
uint32_t imp_timestamp;

//Offsets de los sensores {Steer,APPS1,APPS2,Brake}
struct offsets_t offset;

//Estructura de lectura para el ADC
uint32_t adcReadings[4]; //32*3, el adc saca 12 bits alineados a la derecha

void initPedal(ADC_HandleTypeDef* hadc) {
	adc = hadc;
	ee_init(); //Inicializamos la flash (EEPROM virtual)

	//Carga de los offsets
	ee_read(0, sizeof(offset), (uint8_t*) &offset); //Lee de memoria el struct

	//Check if there are offsets written in flash
	if (!offset.written) { // En un futuro lo ideal sería ver que los valores están en rangos lógicos
		offset.high[0] = 4096; //Valores por defecto
		offset.high[1] = 4096;
		offset.high[2] = 4096;
		offset.high[3] = 4096;
		offset.low[0] = 0;
		offset.low[1] = 0;
		offset.low[2] = 0;
		offset.low[3] = 0;

		offset.written = 1; // Establece un byte en memoria que indica que la placa ha sido programada
		ee_writeToRam(0, sizeof(offset), (uint8_t*) &offset);
		ee_commit();
	}

	//Inicializamos el DMA para que copie nuestros datos al buffer de lecturas
	//Hemos desactivado las interrupciones del mismo en el NVIC para que no obstruyan, solo nos interesa que anden disponibles
	HAL_ADC_Start_DMA(adc, adcReadings, 4); // Arrancamos el ADC en modo DMA

}

void readSensors() {

	//Se leen y convierten las señales
	TeR.bpps.bpps = map(adcReadings[3], offset.low[3], offset.high[3], 0, 255); //Lectura del PRESUROMETRO
	TeR.apps.apps_2 = map(adcReadings[2], offset.low[2], offset.high[2], 0,
			255); //Lectura de APPS1
	TeR.apps.apps_1 = map(adcReadings[1], offset.low[1], offset.high[1], 0,
			255); //Lectura del APPS2
	TeR.steer.angle = map(adcReadings[0], offset.low[0], offset.high[0],
	MAXWHEELANGLE, -MAXWHEELANGLE); //Lectura ANGULO de giro (Poner factor)

	//Computa la media
	TeR.apps.apps_av = TeR.apps.imp_flag ? 0 :(TeR.apps.apps_2 + TeR.apps.apps_1) / 2;

	//Check for implausability
	if (abs(TeR.apps.apps_1 - TeR.apps.apps_2) > 255 * 10 / 100) { //T 11.8.9 Desviacion de 10 puntos en %
		if (imp_timestamp == 0) {	 //Si no había timestamp activalo
			imp_timestamp = HAL_GetTick();
		} else if (HAL_GetTick() - imp_timestamp > 100) {//Si el tiempo es mayor que 100 millis
			TeR.apps.imp_flag = 1; //Activa el implausability y dejalo latched
			imp_timestamp = 0; //Resetea el counter
		}
	} else { //Si vuelve a estar bien desactiva el contador
		imp_timestamp = 0;
	}
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, TeR.apps.imp_flag); //Actualizamos el estado del led
}

int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min,
		int32_t out_max) {
	//Saturar las salidas si la entrada excede el límite de calibracion
	if (x < in_min)
		return out_min;
	if (x > in_max)
		return out_max;
	//Mapear si estamos en rango seguro
	long val = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	return val;
}

