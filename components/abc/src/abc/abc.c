/*
 * abc.c
 *
 *  Created on: 19 nov. 2021
 *      Author: ABCD
 */




#include "abc.h"

#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include <string.h>
#include <unistd.h>
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "sdkconfig.h"
#include <math.h>


#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                /*!< I2C master read */
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1                            /*!< I2C nack value */
#define RST_FLAG                        (1<<7)








// borrar, ver si es completamente inutil y borrar.
//unsigned char multiMedidas_f;     // cuando se mide capacitor nivel y ademas capacitores referencia liquido aire.


i2c_port_t i2c_num_g;	// global, para que todas las funciones usen este y no necesiten tomarlo como parametro.

int mode_g, sampleRate_g, capdac_offset_g1, capdac_offset_g3, capdac_offset_g4;  // Globales, para recargar la configuracion anterior en reset.

int errorr;

enum tipo_medida tipoMedida_g;  // para recargar config ante un reset.

enum cap_config{
	SINGLE=0,
	REPEAT=0x100
};


enum {							//Direcciones de los registros de configuracion de las medidas:
    regDIR_CONF_MEAS1= 0x8,
    regDIR_CONF_MEAS2= 0x9,
    regDIR_CONF_MEAS3= 0xA,
    regDIR_CONF_MEAS4= 0xB
};














//Prototipos_________________________________________________________________________



float mean(int* muestras, int tam);



float deviation(int* muestras, int tam);



// Lee el registro recibido como parametro.
//      file_p              : Puntero al bus I2C.
//      rxBuffer            : Puntero al array de datos donde se guarda lo recibido por I2C.
//      txBuffer            : Puntero al array de datos a enviar por I2C.
//      reg		            : Registro fuente de la lectura.
// Retorna:
//      0 : No hubo error.
//     -1 : Fallo.
int capacimeter_read(i2c_port_t i2c_num, uint8_t* rxBuffer_p, uint8_t* txBuffer_p, enum reg_resultado reg);




// Escribe en un registro del FDC1004.
// Parametros:
//      file_p              : Puntero al bus I2C.
//      rxBuffer            : Puntero al array de datos donde se guarda lo recibido por I2C.
//      txBuffer            : Puntero al array de datos a enviar por I2C.
//      reg                 : Registro destino de la escritura.
// Retorna:
//      0 : No hubo error.
//     -3 : Fallo en la escritura i2c.
int capacimeter_write(i2c_port_t i2c_num, uint8_t* rxBuffer_p, uint8_t* txBuffer_p, int reg);




// Lee el flag de conversion completa, y retorna su valor, o informacion del error en caso que ocurra.
//      file_p              : Puntero al bus I2C.
// Retorna:
//      0 : La medida no esta lista para ser leida.
//      1 : La medida esta lista para ser leida.
//     -3 : Fallo en la escritura i2c.
//     -4 : Fallo en la lectura i2c.
int capacimeter_done(i2c_port_t i2c_num, enum tipo_medida tipoMedida);



























//Funciones____________________________________________________________________________




int capacimeter_read(i2c_port_t i2c_num, uint8_t* rxBuffer_p, uint8_t* txBuffer_p, enum reg_resultado reg){         // Parametro es el reg a leer



	//ESCRIBO EL POINTER REG ANTES DE LEER:

	int ret;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, FDC1004_SENSOR_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        return -1;
    }

    usleep(50);




    //LEO LO APUNTADO POR POINTER REG:

	cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, FDC1004_SENSOR_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, rxBuffer_p, ACK_VAL);
    i2c_master_read_byte(cmd, rxBuffer_p+1, NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    if(ret != ESP_OK)
    	return -1;
    return 0;
}









int capacimeter_write(i2c_port_t i2c_num, uint8_t* rxBuffer_p, uint8_t* txBuffer_p, int reg){          //Llenar el buffer antes-> tx[1]= MSB, tx[2]=LSB (tx[0] es el reg, parametro recibido, no hace falta llenar tx[0])




	txBuffer_p[0]= reg;
	int ret;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
    i2c_master_write_byte(cmd, FDC1004_SENSOR_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write(cmd, txBuffer_p, 3, ACK_CHECK_EN);
    //i2c_master_write_byte(cmd, reg, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        return -1;
    }

    usleep(50);

    return 0;
}









int capacimeter_init(i2c_port_t i2c_num, int capdac_offset, enum sample_rate sampleRate){    //original

    i2c_num_g= i2c_num;		// guardo en variable global, asi no hay que pasar como parametro al resto de las funciones.


    //if(capacimeter_config(capdac_offset, sampleRate, medidaNIVEL)!=0){              // por defecto inicia habilitada una sola medicion. original
    MEASn_capdac_config(capdac_offset, medidaNIVEL);                                  //nueva


    if(capacimeter_config(sampleRate, medidaNIVEL)!=0){                               //nueva
    	printf("capacimeter_config failed.\n");
    	return -2;
    }


    return 0;
}










int capacimeter_config(enum sample_rate sampleRate, enum tipo_medida tipoMedida){


    // modificada para que no configure los offsets. con varios canales conviene modif. en funcion nueva.

    uint8_t txBuffer[3];
    uint8_t rxBuffer[3];

    //multiMedidas_f= (tipoMedida==medidaDIFERENCIAL) ? 1 : 0;    // Este flag lo uso por fuera para saber en que
                                                                // modo esta.


    ///config global para todos los tipos de mediciones:



    //guardo en globales para reconfigurar ante un reset.
    mode_g= REPEAT;
    sampleRate_g= sampleRate;
    tipoMedida_g= tipoMedida;


    /*      //original:
    ///config meas1 (medicion de cap de nivel):
    capdac_offset_g1= capdac_offset;

    u_int16_t reg_CONF_MEAS1_= capdac_offset<<5;
    reg_CONF_MEAS1_|= 0x1000; // capdac enabled.

    //Configuracion para mediciones:
    txBuffer[1]= (reg_CONF_MEAS1_ & 0xFF00)>>8; //MSB     //txBuffer[1]
    txBuffer[2]= (reg_CONF_MEAS1_ & 0xFF  );    //LSB     //Configuro el registro 0x08. txBuffer[2]

    //capacimeter(WRITE, 0x08);
    if( capacimeter_write(&fs_nivel, rxBuffer, txBuffer, 0x08) != 0 ){
        printf("Failed capacimeter_write first call, in capacimeter_config.\n");
        return -5;
    }
  original
*/






    // PINES DE MEDIDA DIFERENCIAL ESTAN EN EL HEADER.
    ///config meas2 (medicion diferencial capacitor liquido - capacitor aire):
    u_int16_t reg_CONF_MEAS2_= (DIF_MEASURE_POSITIVE_PIN<<13) | (DIF_MEASURE_NEGATIVE_PIN<<10); //| (CHA->cin2) | (CHB->cin3)    // modifique y no tenia que. corregir.

    //Configuracion para mediciones:
    txBuffer[1]= (reg_CONF_MEAS2_ & 0xFF00)>>8; //MSB     //txBuffer[1]
    txBuffer[2]= (reg_CONF_MEAS2_ & 0xFF  );    //LSB     //Configuro el registro . txBuffer[2]

    //capacimeter(WRITE, );
    if( capacimeter_write(i2c_num_g, rxBuffer, txBuffer, regDIR_CONF_MEAS2) != 0 ){
        printf("Failed capacimeter_write first call, in capacimeter_config.\n");
        return -5;
    }






    ///inicio las mediciones:
    //config de reg 0x0C
    //u_int16_t reg_0x0C_= sampleRate | REPEAT | ( (tipoMedida==medidaDIFERENCIAL) ? (0x80 | 0x40) : 0x80 );      // 0x80->meas1 enable. 0x40->meas2 enable. original

    //a diferencia de antes ahora se hace de a una sola medida, sea diferencial o no.


    u_int16_t reg_0x0C_= sampleRate | REPEAT | (tipoMedida<<4);     // habilito solo uno.

    txBuffer[1]= (reg_0x0C_ & 0xFF00)>>8; //MSB
    txBuffer[2]= (reg_0x0C_ & 0xFF  );    //LSB         //Configuro el registro 0x0C




    //capacimeter(WRITE, 0x0C);
    if( capacimeter_write(i2c_num_g, rxBuffer, txBuffer, 0x0C) != 0 ){
        printf("Failed capacimeter_write call, in capacimeter_config.\n");
        return -5;
    }

    return 0;
}





int MEASn_capdac_config(int capdac_offset, enum tipo_medida tipoMedida){

    uint8_t txBuffer[3];
    uint8_t rxBuffer[3];
    unsigned char pinCIN;
    unsigned char regDIR_CONF_MEASn;



    //guardo en variables globales las configuraciones en caso de reset (reset reconfigura desp del reset)
    //ademas selecciono el registro donde se guardara la configuracion:

    switch (tipoMedida){

        case medidaNIVEL:
            capdac_offset_g1= capdac_offset;
            pinCIN= pinCIN1;
            regDIR_CONF_MEASn= regDIR_CONF_MEAS1;
            break;

        case medidaMEAS3:
            capdac_offset_g3= capdac_offset;
            pinCIN= pinCIN3;
            regDIR_CONF_MEASn= regDIR_CONF_MEAS3;
            break;

        case medidaMEAS4:
            capdac_offset_g4= capdac_offset;
            pinCIN= pinCIN4;
            regDIR_CONF_MEASn= regDIR_CONF_MEAS4;
            break;

        default:
            return -1;

    }





    //escribo registro de configuracion por i2c:
    u_int16_t reg_CONF_MEASn_= capdac_offset<<5;
    reg_CONF_MEASn_|= 0x1000; // capdac enabled.
    reg_CONF_MEASn_|= (pinCIN<<13); //| (CHA->cin)

    //Configuracion para mediciones:
    txBuffer[1]= (reg_CONF_MEASn_ & 0xFF00)>>8; //MSB
    txBuffer[2]= (reg_CONF_MEASn_ & 0xFF  );    //LSB

    //capacimeter(WRITE, dir);
    if( capacimeter_write(i2c_num_g, rxBuffer, txBuffer, regDIR_CONF_MEASn) != 0 ){
        printf("Failed capacimeter_write first call, in capacimeter_config.\n");
        return -5;
    }


    return 0;
}









int capacimeter_done(i2c_port_t i2c_num, enum tipo_medida tipoMedida){ // retorna el estado de la conversion (1 o 0) y <0 en caso de error.

    uint8_t txBuffer[3];
    uint8_t rxBuffer[3];

    usleep(50);

    if(capacimeter_read(i2c_num_g, rxBuffer, txBuffer, 0x0C) != 0){
    	printf("Failed to write to the i2c bus.\n");
    	return -1;

    }else{
        return tipoMedida & rxBuffer[1];    // tipo medida es la mascara del bit done del registro C.
    }

}














int read_autoranging_cap_pF(float* capacidad, enum tipo_medida tipoMedida){         //nueva


    float cap;
    unsigned char saltoOffset= 0b01000;                 // busqueda dicotomica, salto se va a ir dividiendo por 2
    unsigned char capdac_offset= 0b10000;               //    0 (0pF) < capdac (5 bits) < 31 (96.875pF)
    //unsigned char multiMedidas= multiMedidas_f;         // si antes estaba configurado para multiples medidas, guardo aca y al finalizar la funcion restauro la config.
    //unsigned char fueraDeRangoSuperior;                 // si se va de rango completo, con esto se si fue fuera del limite superior o inferior.

    MEASn_capdac_config(capdac_offset, tipoMedida);     //nueva
    capacimeter_config(CUATROCIENTAS_Ss, tipoMedida);   //modificado del original



    usleep(3000);   //tiempo mayor a un Ts. // primer medida da muy alto, no funciona. descartar primer medida? : lo de descartar la primer medida funciona.
    read_single_cap_pF(&cap, tipoMedida);  //descarto la primer muestra -> funciono. modificado del original
    //en lugar de descartar la primer muestra, podria esperar mas ms? despues de capacimeter_config hay que esperar, hoja de datos no dice.

    saltoOffset*=2; //desplazo a izda para compenzar el primer desplazamiento a dcha dentro del bucle.

    while(1){     //mientras este saturado: corrige offset.			// peor caso: 5 iteraciones (la 6 retorna antes de ejecutar el delay y todo eso)


        if(saltoOffset==0){
            *capacidad= cap;
            // reconfiguro tipo de medicion que habia antes de entrar a esta funcion:
            //multiMedidasEnable(multiMedidas);
            return capdac_offset;
        }

        saltoOffset/= 2;

        //capacimeter_config(capdac_offset, CUATROCIENTAS_Ss, medidaNIVEL); //original
        MEASn_capdac_config(capdac_offset, tipoMedida);     // reconfiguro offset
        usleep(6000);   //si este delay es de 3ms, mide mal, porque toma una medida con el offset anterior, y no el offset que configure en la linea anterior "MEASn_capdac_conf...".

        read_single_cap_pF(&cap, tipoMedida);      // modif del original



        //chequeo si no saturo, si saturo modifico el offset:
        if(15.998<cap){
            capdac_offset+= saltoOffset;
        }else if(cap<-15.998){
            capdac_offset-= saltoOffset;

        }else{
            *capacidad= cap;     // si entro aca, no saturo, fin.

            // reconfiguro en la forma que habia antes de entrar a esta funcion:
            //multiMedidasEnable(multiMedidas);
            return capdac_offset;
        }


    }


}















int read_single_cap_pF(float* cap, enum tipo_medida tipoMedida){


    uint8_t txBuffer[3];
    uint8_t rxBuffer[3];
    int retn;
    int err;
    int medida=0;




    // REGISTRO A LEER:
    unsigned char registroResultado;

    switch (tipoMedida){

        case medidaNIVEL:
            registroResultado= reg_RESULTADO_NIVEL;
            break;

        case medidaDIFERENCIAL:
            registroResultado= reg_RESULTADO_DIFERENCIAL;
            break;

        case medidaMEAS3:
            registroResultado= reg_RESULTADO_MEAS3;
            break;

        case medidaMEAS4:
            registroResultado= reg_RESULTADO_MEAS4;
            break;

        default:
        	registroResultado= reg_RESULTADO_NIVEL;
            break;

    }




    retn= capacimeter_done(i2c_num_g, tipoMedida);
    if(retn<0){
        printf("error leyendo flag done.\n");
        return -9;
    }
    if(retn==0){

        //usleep( multiMedidas_f ? 5500 : 3000 );   //esperar 3ms y reintentar
    	usleep(3000);

        retn= capacimeter_done(i2c_num_g, tipoMedida);  //cambiar
        if(retn==0){
            printf("flag conversion completa no se setea.\n");
            err= reset(i2c_num_g);
            if(err!=0){
                printf("error en reset.\n");
                return -8;
            }
            return -10;
        }
        if(retn<0){
            printf("error leyendo flag done.\n");
            err= reset(i2c_num_g);
            if(err!=0){
                printf("error en reset.\n");
                return -8;
            }
            return -9;
        }
    }



    //Leo:
    //capacimeter(READ, );       //Leo registro alto
    if( (capacimeter_read(i2c_num_g, rxBuffer, txBuffer, registroResultado)) != 0 ){  //cambiar
        printf("Error en lectura de registro.\n");
        return -7;
    }
    medida= (*(rxBuffer) << 16) + (*(rxBuffer+1) << 8);

    //capacimeter(READ, );        //Leo registro bajo
    if( (capacimeter_read(i2c_num_g, rxBuffer, txBuffer, registroResultado+1)) != 0 ){    //cambiar
        printf("Error en lectura de registro.\n");
        return -7;
    }
    medida= medida + *rxBuffer;   //Los 8LSB son reservados, no son parte del resultado (rxBuffer[1] no interesa).

    // CONVERTIR MEDIDA A STRING:

    //SIGNO:
    if( medida & 0x800000 ){
        medida= ~medida+1;
        medida= medida & 0xffffff;
        medida= -medida;
    }
    //SIGNO.

    *cap= (float)medida/(1<<19);


    return 0;

}




int reset(i2c_port_t i2c_num){

	uint8_t txBuffer[3];
	uint8_t rxBuffer[3];
	uint8_t capdac_offset;



    switch (tipoMedida_g){

        case medidaNIVEL:
            capdac_offset= capdac_offset_g1;
            break;

        case medidaMEAS3:
            capdac_offset= capdac_offset_g3;
            break;

        case medidaMEAS4:
            capdac_offset= capdac_offset_g4;
            break;

        default:
        	capdac_offset= 0;
            break;

    }



    // Reset:
    txBuffer[1] = RST_FLAG;     //msb= rst
    txBuffer[2] = 0x80;         //lsb no importa, tras el reset hay que reconfigurar el ic.
    int err= capacimeter_write(i2c_num, rxBuffer, txBuffer, 0x0C);    // Envia reset.
    if(err!=0){
        printf("error reseteando.\n");
        return -1;
    }
    usleep(100);

    MEASn_capdac_config(capdac_offset, tipoMedida_g);
    err= capacimeter_config(sampleRate_g, tipoMedida_g);  // reconfiguro por el reset.
    if(err!=0){
        printf("error reconfigurando.\n");
        return -2;
    }

    return 0;
}









int read_processed_cap_pF(enum tipo_medida tipoMedida, float desviacion_aceptable_pF, int vectorSize, mean_reliability *salidaStruct_p){

    uint8_t txBuffer[3];
    uint8_t rxBuffer[3];
    int medida=0;
    int err;
    int retn;
    int muestras[vectorSize];
    float desviacion_aceptable= (desviacion_aceptable_pF)*(1<<19);    //Convierto pF a cuentas del adc.





    unsigned char regResultado;

    switch (tipoMedida){

        case medidaNIVEL:
            regResultado= reg_RESULTADO_NIVEL;
            break;

        case medidaDIFERENCIAL:
            regResultado= reg_RESULTADO_DIFERENCIAL;
            break;

        case medidaMEAS3:
            regResultado= reg_RESULTADO_MEAS3;
            break;

        case medidaMEAS4:
            regResultado= reg_RESULTADO_MEAS4;
            break;

        default:
        	regResultado= reg_RESULTADO_NIVEL;
            break;

    }




    // TOMO LAS N MUESTRAS:
    for(int i=0; i<vectorSize; i++){



        retn= capacimeter_done(i2c_num_g, tipoMedida);  //cambiar
        if(retn<0){
            printf("error leyendo flag done.\n");
            return -9;
        }
        if(retn==0){
            //usleep( multiMedidas_f ? 5500 : 3000);   //esperar 3ms y reintentar (si se toman dos tipos de medidas) (cambiar)
            usleep(3000);
            retn= capacimeter_done(i2c_num_g, tipoMedida);  //cambiar
            if(retn==0){
                printf("flag conversion completa no se setea.\n");
                err= reset(i2c_num_g);
                if(err!=0){
                    printf("error en reset.\n");
                    return -8;
                }
                return -10;
            }
            if(retn<0){
                printf("error leyendo flag done.\n");
                err= reset(i2c_num_g);
                if(err!=0){
                    printf("error en reset.\n");
                    return -8;
                }
                return -9;
            }
        }



        //Leo:
        //capacimeter(READ, 0);       //Leo registro alto reg0
        if((capacimeter_read(i2c_num_g, rxBuffer, txBuffer, regResultado)) != 0){    //cambiar
            printf("Error en lectura de registro en read_processedData.\n");
            return -7;
        }
        medida= (rxBuffer[0] << 16) + (rxBuffer[1] << 8);

        //capacimeter(READ, 1);        //Leo registro bajo reg1
        if( (capacimeter_read(i2c_num_g, rxBuffer, txBuffer, regResultado+1)) != 0 ){    //cambiar
            printf("Error en lectura de registro en read_processedData.\n");
            return -7;
        }
        medida= medida + *rxBuffer;   //Los 8LSB son reservados, no son parte del resultado (rxBuffer[1] no interesa).

        muestras[i]= medida;

    }               //YA ESTAN TOMADAS LAS MUESTRAS. PROCESAR:

    float media;
    float desviacion;
    int muestrasUtiles[vectorSize];       // Inicialmente es el vector de medidas.
    for(int i=0; i<vectorSize; i++){
        muestrasUtiles[i]= muestras[i];
    }
    int muestrasUtilesBuffer[vectorSize];
    int cantMuestrasUtiles=vectorSize;         //Inicialmente, todas son utiles.

    int j=0;
    int muestra_signada;
    while(1){      //Se hace iteraciones hasta obtener el vector final con desviacion<desviacion_aceptable.

        if( (desviacion= deviation(muestrasUtiles, cantMuestrasUtiles)) < 0 ){
            printf("Error en calculo de desviacion en read_processedData.\n");
            return -11;
        }
        errorr=0;
        media= mean(muestrasUtiles, cantMuestrasUtiles);
        if(errorr){
            printf("Error en calculo de media en read_processedData.\n");
            return -12;
        }

        if(desviacion<desviacion_aceptable)
            break;
        if(cantMuestrasUtiles<=2)
            break;


        for(int i=0; i<cantMuestrasUtiles; i++){
            muestrasUtilesBuffer[i]=muestrasUtiles[i];
        }
        j=0;

        for(int i=0; i<cantMuestrasUtiles; i++){

            if( muestrasUtilesBuffer[i] & 0x800000 ){
                muestra_signada= ~muestrasUtilesBuffer[i]+1;
                muestra_signada= muestra_signada & 0xffffff;
                muestra_signada= -muestra_signada;
            }else{
                muestra_signada= muestrasUtilesBuffer[i];
            }


            if( abs(muestra_signada-media)<desviacion ){
                muestrasUtiles[j]=muestrasUtilesBuffer[i];
                j++;
            }

        }
        cantMuestrasUtiles=j;
    }


    float capacidad= (float)media/(1<<19);



    salidaStruct_p->mean         = capacidad;
    salidaStruct_p->reliability = cantMuestrasUtiles*100/vectorSize;    // (cantMuestrasUtiles/totales)*100%.

    return 0;
}











float mean(int* muestras, int tam){

	errorr= 0;
    if(tam<1){
        printf("Error intento de division por cero en calcularMedia.\n");
        errorr=1;
        return -11;
    }

    float media= 0;
    int muestra_signada;

    for(int i=0; i<tam; i++){
        if( *(muestras+i)&0x800000 ){
            muestra_signada= ~(*(muestras+i))+1;
            muestra_signada= muestra_signada & 0xffffff;
            muestra_signada= -muestra_signada;
        }else{
            muestra_signada= *(muestras+i);
        }
        media+= muestra_signada;
    }

    return (media/tam);
}







float deviation(int* muestras, int tam){

    if(tam<2){
        printf("Error de division por cero en calcularDesviacion.\n");
        return -11;
    }

    float aux_sumatoria= 0;
    errorr=0;
    float media_muestral= mean(muestras, tam);
    if(errorr){
        printf("Error en calcularMedia en calcularDesviacion.\n");
        return -12;
    }
    int muestra_signada;
    for(int i=0; i<tam; i++){

        if( *(muestras+i)&0x800000 ){
            muestra_signada= ~(*(muestras+i))+1;
            muestra_signada= muestra_signada & 0xffffff;
            muestra_signada= -muestra_signada;
        }else{
            muestra_signada= *(muestras+i);
        }

        aux_sumatoria+= ( muestra_signada-media_muestral) * ( muestra_signada-media_muestral);
    }
    aux_sumatoria= aux_sumatoria/(tam-1);     // Media muestral-> /(n-1)

    return sqrt(aux_sumatoria);
}








// 13 funciones:


//capacimeter_init											LISTO

//capacimeter_config										LISTO

//capacimeter_read											LISTO

//capacimeter_write											LISTO

//capacimeter_done											LISTO

//MEASn_capdac_config										LISTO

//multiMedidasEnable										no necesaria por ahora

//read_processed_cap_pF										LISTO

//capacidad_autooffset renombrar a read_autoranging_cap_pF

//capacidad_medida_single renombrar a read_single_cap_pF	LISTO falta renombrar

//reset														LISTO

//calcularDesviacion		renombrada a deviation			LISTO

//calcularMedia				renombrada a mean				LISTO



