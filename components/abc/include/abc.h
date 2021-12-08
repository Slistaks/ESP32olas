/*
 * abc.h
 *
 *  Created on: 19 nov. 2021
 *      Author: ABCD
 */

#ifndef MAIN_ABC_H_
#define MAIN_ABC_H_



#define FDC1004_SENSOR_ADDR 0x50





//pongo aca pero cuando funcione va al .c

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include <string.h>
#include <unistd.h>
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "sdkconfig.h"



// El pin positivo debe ser un numero de pin menor a el numero de pin negativo (si pin positivo es cin2, pin negativo solo puede ser cin3 o cin4).
#define DIF_MEASURE_POSITIVE_PIN pinCIN2
#define DIF_MEASURE_NEGATIVE_PIN pinCIN3



/// Direcciones de los registros MSB donde se almacenan los resultados de las mediciones:
enum reg_resultado{
    reg_RESULTADO_NIVEL= 0,                        // Uso el MEAS1
    reg_RESULTADO_DIFERENCIAL=2,                   // Uso el MEAS2
    reg_RESULTADO_MEAS3= 4,                        // MEAS3
    reg_RESULTADO_MEAS4= 6                        // MEAS4
};


enum sample_rate{
    CIEN_Ss=0x400,
    DOSCIENTAS_Ss=0x800,
    CUATROCIENTAS_Ss=0xC00
};


enum tipo_medida{
    medidaNIVEL= 8,             // lo uso como mascara en funcion done. Nivel, registro meas1.
    medidaDIFERENCIAL= 4,       // diferencial, registro meas2.
    medidaMEAS3= 2,
    medidaMEAS4= 1
}tipoMedida;



enum {
    pinCIN1= 0,
    pinCIN2= 1,
    pinCIN3= 2,
    pinCIN4= 3
};



/**
 * Estructura usada para guardar la media de las muestras y
 * la relacion entre muestras utiles respecto al total de muestras.
 **/
typedef struct mean_reliability_struct {

    float mean;
    float reliability;// relacion entre cantidad de muestras utiles y cantidad de muestras totales (0%-100%).

}mean_reliability;











//
// Convierte capacidad a milimetros, d[mm]= a*cap^2 + b*cap + c
//
//
float cap_to_mm(float cap, float a, float b, float c);












// capacimeter_init: Inicializa y configura el integrado fdc1004.
//
// Parametros:
//      file_p			: Puntero al bus I2C.
//		capdac_offset	: Offset de capacidad, 3.125pF por unidad. 5 bits (maximo offset: 96.875pF)
//		sampleRate 		: Tasa de muestreo, posibles: 100, 200 y 400 muestras por segundo. Usar enume, ver nivelLib.h.
// Retorna:
//      0 : No hubo error.
//     -1 : Fallo ioctl
//     -2 : Fallo capacimeter_config
int capacimeter_init(i2c_port_t i2c_num, int capdac_offset, enum sample_rate sampleRate);






int MEASn_capdac_config(int capdac_offset, enum tipo_medida tipoMedida);		// configura el capdac para medida MEASn.




// capacimeter_config: Configura el offset del capdac y la tasa de muestreo. Se puede usar en cualquier momento
//					   en que se quiera cambiar esos atributos.
//
// Parametros:
//		capdac_offset: El offset a restar a la capacidad a medir. El offset en pF es capdac_offset*3.125pF.
//		sampleRate   : La tasa de muestreo. Posibles: 100, 200 y 400 muestras por segundo. Usar enum de nivelLib.h.
// Retorna:
//		terminar
//int capacimeter_config(int capdac_offset, enum sample_rate sampleRate, enum tipo_medida tipoMedida);  //original
int capacimeter_config(enum sample_rate sampleRate, enum tipo_medida tipoMedida);






// read_single_cap_pF: Toma una sola medida de capacidad en pF.
//
// VER "NOTA 2" EN SU DEFINICION. En funcion de comentar o no una linea, se puede obtener como salida la
//								  capacidad absoluta, o la capacidad relativa al offset.
//
// Parametros:
//		cap: Puntero a la variable donde se guardara la capacidad en pF.
//
// Retorna:
//		terminar.
int read_single_cap_pF(float* cap, enum tipo_medida tipoMedida);






/*
 *
 *
 * antiguamente llamada auto offset. Encuentra el offset de capacidad (capdac y eso es lo que retorna).
 *
 *
 */
int read_autoranging_cap_pF(float* capacidad, enum tipo_medida tipoMedida);






/*
 *
 * agregar info
 *
 */
int read_processed_cap_pF(enum tipo_medida tipoMedida, float desviacion_aceptable_pF, int vectorSize, mean_reliability *salidaStruct_p);






/**
 * reset: Resetea el integrado fdc1004.
 *
 * Parametros:
 *      file_p: Puntero al file descriptor correspondiente.
 *
 * Retorna:
 *       0: Exito
 *      -1: Fallo capacimeter_write
 *      -2: Fallo en capacimeter_config
 *
 **/
int reset(i2c_port_t i2c_num);











#endif /* MAIN_ABC_H_ */
