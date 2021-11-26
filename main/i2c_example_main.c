/* i2c - Example

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   See README.md file to get detailed usage of this example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"

//mqtt headers
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "mqtt_client.h"
//mqtt headers


#include "sdkconfig.h"
#include "abc.h"
//#include "components/bib/include/abc.h"


static const char *TAG = "i2c-example";

#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)

#define DATA_LENGTH 512                  /*!< Data buffer length of test buffer */
#define RW_TEST_LENGTH 128               /*!< Data length for r/w test, [0,DATA_LENGTH] */
#define DELAY_TIME_BETWEEN_ITEMS_MS 1000 /*!< delay time between different test items */

#define I2C_SLAVE_SCL_IO CONFIG_I2C_SLAVE_SCL               /*!< gpio number for i2c slave clock */
#define I2C_SLAVE_SDA_IO CONFIG_I2C_SLAVE_SDA               /*!< gpio number for i2c slave data */
#define I2C_SLAVE_NUM I2C_NUMBER(CONFIG_I2C_SLAVE_PORT_NUM) /*!< I2C port number for slave dev */
#define I2C_SLAVE_TX_BUF_LEN (2 * DATA_LENGTH)              /*!< I2C slave tx buffer size */
#define I2C_SLAVE_RX_BUF_LEN (2 * DATA_LENGTH)              /*!< I2C slave rx buffer size */

#define I2C_MASTER_SCL_IO CONFIG_I2C_MASTER_SCL               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO CONFIG_I2C_MASTER_SDA               /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUMBER(CONFIG_I2C_MASTER_PORT_NUM) /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ CONFIG_I2C_MASTER_FREQUENCY        /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0                           /*!< I2C master doesn't need buffer */

#define BH1750_SENSOR_ADDR CONFIG_BH1750_ADDR   /*!< slave address for BH1750 sensor */

//#define FDC1004_SENSOR_ADDR 0x50
#define FDC1004_SENSOR_REG_ADDR_MANUFACTURER 0xFE	// este registro deberia leerse: 0x5449

#define BH1750_CMD_START CONFIG_BH1750_OPMODE   /*!< Operation mode */
#define ESP_SLAVE_ADDR CONFIG_I2C_SLAVE_ADDRESS /*!< ESP32 slave address, you can set any 7bit value */
#define WRITE_BIT I2C_MASTER_WRITE              /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ                /*!< I2C master read */
#define ACK_CHECK_EN 0x1                        /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0                       /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                             /*!< I2C ack value */
#define NACK_VAL 0x1                            /*!< I2C nack value */

SemaphoreHandle_t print_mux = NULL;

/**
 * @brief test code to read esp-i2c-slave
 *        We need to fill the buffer of esp slave device, then master can read them out.
 *
 * _______________________________________________________________________________________
 * | start | slave_addr + rd_bit +ack | read n-1 bytes + ack | read 1 byte + nack | stop |
 * --------|--------------------------|----------------------|--------------------|------|
 *
 * @note cannot use master read slave on esp32c3 because there is only one i2c controller on esp32c3
 */
static esp_err_t __attribute__((unused)) i2c_master_read_slave(i2c_port_t i2c_num, uint8_t *data_rd, size_t size)
{
    if (size == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | READ_BIT, ACK_CHECK_EN);
    if (size > 1) {
        i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief Test code to write esp-i2c-slave
 *        Master device write data to slave(both esp32),
 *        the data will be stored in slave buffer.
 *        We can read them out from slave buffer.
 *
 * ___________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write n bytes + ack  | stop |
 * --------|---------------------------|----------------------|------|
 *
 * @note cannot use master write slave on esp32c3 because there is only one i2c controller on esp32c3
 */
static esp_err_t __attribute__((unused)) i2c_master_write_slave(i2c_port_t i2c_num, uint8_t *data_wr, size_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}





/**
 * @brief test code to operate on BH1750 sensor
 *
 * 1. set operation mode(e.g One time L-resolution mode)
 * _________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write 1 byte + ack  | stop |
 * --------|---------------------------|---------------------|------|
 * 2. wait more than 24 ms
 * 3. read data
 * ______________________________________________________________________________________
 * | start | slave_addr + rd_bit + ack | read 1 byte + ack  | read 1 byte + nack | stop |
 * --------|---------------------------|--------------------|--------------------|------|
 */







/*
 * Funciones MQTT
 *___________________________________________________________________________________________________________________________________________
 */





static void log_error_if_nonzero(const char * message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}



esp_mqtt_client_handle_t client;

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    //esp_mqtt_client_handle_t client = event->client;
	client = event->client;

    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {

        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            //msg_id = esp_mqtt_client_publish(client, "/topic/nivel/sensor_n", "Sensor n conectado", 0, 1, 0);

            //ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

            //msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);					// no necesita suscribirse.
            //ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            //msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);					// no necesita suscribirse.
            //ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

            //msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");					// no se suscribio, entonces no se des-suscribe.
            //ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        //case MQTT_EVENT_SUBSCRIBED:
            //ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            //msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            //ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
            //break;

        /*
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
            */

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;

        /*
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
            */

        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

            }
            break;

        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}






static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}









static void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = CONFIG_BROKER_URL,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}







/*
 * Funciones MQTT
 *_______________________________________________________________________________________________________________________________________________________
 */









/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)				// la pase dentro de capacimeter_config()
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
        // .clk_flags = 0,          //!< Optional, you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here.
    };
    esp_err_t err = i2c_param_config(i2c_master_port, &conf);
    if (err != ESP_OK) {
        return err;
    }
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}





static void vLevelMeasureTask(void *arg)
{

    uint32_t task_idx = (uint32_t)arg;



    uint8_t capdac= 0;
    uint8_t sampleNumber= 0;		// de 0 a 50
    uint32_t packetID= 0;
    char packetID_str[11];

    float capacidad[50];
    float desviacionAceptable= 0.2;
    uint8_t cantMuestras= 4;
    mean_reliability estructuraResultado;

    char capacidad_str[7];	//"ccc.cc\0"
    char dataToPublish[352];						// el formato a enviar: "[ccc.cc,ccc.cc,...,...]\0" -> 50 medidas -> 349+3 = 352.


    capacimeter_init(I2C_MASTER_NUM, capdac, CUATROCIENTAS_Ss);		// antes abria el puerto, ahora unicamente ejecuta la configuracion inicial (meas_conf y cap_config).
    usleep(50000);


    while (1) {

        //ESP_LOGI(TAG,"");
    	//ESP_LOGE(TAG, "I2C Timeout");
    	//ESP_LOGW(TAG, "%s", esp_err_to_name(ret));

        xSemaphoreTake(print_mux, portMAX_DELAY);



        // medida de nivel MEDIA DE VARIAS
        //capacimeter_config(etc);
        //read_processed_cap_pF(medidaNIVEL, desviacionAceptable, cantMuestras, &estructuraResultado);
        //printf("media: %0.2f\nconfiabilidad: %0.2f\n", estructuraResultado.mean+3.125*capdac, estructuraResultado.reliability);


        // medida de nivel AUTORANGO:
        capacimeter_config(CUATROCIENTAS_Ss, medidaNIVEL);
        usleep(8000);
        capdac= read_autoranging_cap_pF(&capacidad[sampleNumber], medidaNIVEL);
        //printf("medida %d\ncapacidad: %0.2f\ncapdac: %d\n", sampleNumber, capacidad[sampleNumber]+capdac*3.125, capdac);


        // medida diferencial MEDIDA UNICA:
        //capacimeter_config(CUATROCIENTAS_Ss, medidaDIFERENCIAL);
        //usleep(8000);
        //read_single_cap_pF(&capacidad, medidaDIFERENCIAL);
        //printf("capacidad diferencial: %0.2f<<<<<<<<\n", capacidad);






		sampleNumber++;

		if(49<sampleNumber){

			strcpy(dataToPublish, "[");			// no me deja poner dentro de sprintf..
			sprintf(packetID_str, "%d", packetID++);
			strcat(dataToPublish, packetID_str);

			for(int i=0; i<sampleNumber; i++){
				strcat(dataToPublish, ",");
				sprintf(capacidad_str, "%.2f", capacidad[i]+3.125*capdac);
				strcat(dataToPublish, capacidad_str);
			}

			strcat(dataToPublish, "]");			// no me deja poner dentro de sprintf..
			printf("####################\ndatos a publicar: %s\n######################\n", dataToPublish);
			esp_mqtt_client_publish(client, "/topic/nivel/sensor_n", dataToPublish, 0, 1, 0);
			sampleNumber= 0;

		}





        xSemaphoreGive(print_mux);
        //vTaskDelay((DELAY_TIME_BETWEEN_ITEMS_MS * (task_idx + 1)) / portTICK_RATE_MS);
        vTaskDelay(20/portTICK_RATE_MS);
    }




    vSemaphoreDelete(print_mux);
    vTaskDelete(NULL);

}























void app_main(void)
{
    print_mux = xSemaphoreCreateMutex();

    //mqtt
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_TCP", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_SSL", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	ESP_ERROR_CHECK(example_connect());	// This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
										// Read "Establishing Wi-Fi or Ethernet Connection" section in examples/protocols/README.md for more information about this function.

	mqtt_app_start();
    //mqtt


    ESP_ERROR_CHECK(i2c_master_init());


    xTaskCreate(vLevelMeasureTask, "vLevelMeasureTask_0", 1024 * 8, (void *)0, 10, NULL);
    //xTaskCreate(vLevelMeasureTask, "vLevelMeasureTask_1", 1024 * 2, (void *)1, 10, NULL);


}

















