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
persist_t DELTA_IMP;
persist_t RANGE_IMP;
uint8_t impRange; //1 if signals are out of range
uint8_t impDelta; //1 if APPS difference exceeds 10% of total range

//Offsets de los sensores {Steer,APPS1,APPS2,Brake}
struct offsets_t offset;

//Estructura de lectura para el ADC
int32_t adcReadings[4]; //32*3, el adc saca 12 bits alineados a la derecha usamos enteros con signo pa que no nos la lien las restas al comprobar offsets

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
		offset.low[0] = 0;
		offset.low[1] = 0;
		offset.low[2] = 0;
		offset.written = 1; // Establece un byte en memoria que indica que la placa ha sido programada
		ee_writeToRam(0, sizeof(offset), (uint8_t*) &offset);
		ee_commit();
	}

	//Inicializamos el DMA para que copie nuestros datos al buffer de lecturas
	//Hemos desactivado las interrupciones del mismo en el NVIC para que no obstruyan, solo nos interesa que anden disponibles
	HAL_ADC_Start_DMA(adc, (uint32_t*)adcReadings, 4); // Arrancamos el ADC en modo DMA

}

void readSensors() {

	//Se leen y convierten las señales
	TeR.bpps.bpps = map(adcReadings[3], MINBRAKE, MAXBRAKE, 0, 50); //Estamos con VREF de 5 porque hay un divisor
	TeR.apps.apps_2 = map(adcReadings[2], offset.low[2], offset.high[2], 0,
			255); //Lectura de APPS1
	TeR.apps.apps_1 = map(adcReadings[1], offset.low[1], offset.high[1], 0,
			255); //Lectura del APPS2
	TeR.steer.angle = map(adcReadings[0], offset.low[0], offset.high[0],
	-MAXWHEELANGLE, MAXWHEELANGLE); //Lectura ANGULO de giro (Poner factor)



	//Check for implausability T 11.8.9 Desviacion de 10 puntos en %
	impDelta = !checkPersistance(&RANGE_IMP,(abs(TeR.apps.apps_1 - TeR.apps.apps_2) < (255 * 0.1)),100);//Comprueba que la diferencia entre aceleradores es menor que el 10% activamente, solo falla si esta se da por más de 100ms
	//Check if all signals are in range
	impRange = !checkPersistance(&DELTA_IMP,((adcReadings[1] > (offset.low[1] -MARGIN)) && (adcReadings[2] > (offset.low[2]-MARGIN)) && (adcReadings[3] > (MINBRAKE-MARGIN)) && (adcReadings[1] < (offset.high[1] + MARGIN)) && (adcReadings[2] < (offset.high[2] + MARGIN)) && (adcReadings[3] < (MAXBRAKE + MARGIN))),500); //Implausible range 500 millis
	TeR.apps.imp_flag = (impDelta || impRange); //Determine existing implausability
	//Computa la media
	TeR.apps.apps_av = TeR.apps.imp_flag ? 0 :(TeR.apps.apps_2 + TeR.apps.apps_1) / 2;


	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, TeR.apps.imp_flag); //Actualizamos el estado del led
}

