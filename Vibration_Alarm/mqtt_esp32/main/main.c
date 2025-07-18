#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "driver/gpio.h"
#include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#define VIBRA_GPIO GPIO_NUM_18 
#define BUZZER_GPIO GPIO_NUM_19

static const char *TAG = "MQTT";
#define EXAMPLE_ESP_WIFI_SSID ""      
#define EXAMPLE_ESP_WIFI_PASS "" 
#define MAX_RETRY 10

volatile bool vibracion_detectada = false;

static int retry_cnt = 0;

uint32_t MQTT_CONNECTED = 0;  // Variable corregida

static void mqtt_app_start(void);

// Manejador de eventos Wi-Fi corregido (void)
static void wifi_event_handler(void *arg, esp_event_base_t event_base, 
                              int32_t event_id, void *event_data) {
    switch (event_id) {
        case WIFI_EVENT_STA_START:
            esp_wifi_connect();
            ESP_LOGI(TAG, "Intentando conectar a Wi-Fi");
            break;

        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "Conectado a Wi-Fi");
            break;

        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "IP obtenida: Iniciando cliente MQTT");
            mqtt_app_start();
            break;

        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "Desconectado: Reintentando conexión");
            if (retry_cnt++ < MAX_RETRY) {
                esp_wifi_connect();
            } else {
                ESP_LOGE(TAG, "Máximo de reintentos fallidos");
            }
            break;

        default:
            break;
    }
}

// Función que se ejecuta cuando hay vibración
static void IRAM_ATTR sensor_isr_handler(void* arg) {
   vibracion_detectada = true;
}

void configurar_gpio()
{
    // Configurar el pin como salida
    gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << VIBRA_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE  // Flanco de bajada
    };
    gpio_config(&io_conf);

    gpio_config_t io_conf_buzzer = {
        .pin_bit_mask = (1ULL << BUZZER_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf_buzzer);
}

void wifi_init(void) {
    esp_event_loop_create_default();
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    esp_netif_init();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();
}

// Manejador de eventos MQTT
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, 
                              int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "Conectado al broker MQTT");
            MQTT_CONNECTED = 1;
            
            msg_id = esp_mqtt_client_subscribe(client, "/topic/test1", 0);
            ESP_LOGI(TAG, "Suscripción exitosa, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "/topic/test2", 1);
            ESP_LOGI(TAG, "Suscripción exitosa, msg_id=%d", msg_id);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "Desconectado del broker MQTT");
            MQTT_CONNECTED = 0;
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "Datos recibidos");
            printf("Tópico: %.*s\n", event->topic_len, event->topic);
            printf("Datos: %.*s\n", event->data_len, event->data);
            break;

       
        default:
            break;
    }
}

esp_mqtt_client_handle_t client = NULL;

static void mqtt_app_start(void) {
    ESP_LOGI(TAG, "Iniciando cliente MQTT");
    esp_mqtt_client_config_t mqttConfig = {
        .broker.address.uri = "",  // Cambia esto por tu broker MQTT
    };
    client = esp_mqtt_client_init(&mqttConfig);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);
}


void Publisher_Task(void *params) {
   
        if (MQTT_CONNECTED) {
            esp_mqtt_client_publish(
                client, 
                "/topic/test3", 
                "23",  
                0,  // QoS 0
                0,  // Retain
                0   // Mensaje duplicado
            );
            ESP_LOGI(TAG, "Mensaje publicado");
        }
       
}

void app_main(void) {
 
    configurar_gpio();
    gpio_set_level(BUZZER_GPIO, 0);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(VIBRA_GPIO, sensor_isr_handler, NULL);

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    wifi_init();
   while(1){
        if(vibracion_detectada && MQTT_CONNECTED) {
            gpio_set_level(BUZZER_GPIO, 1);

            esp_mqtt_client_publish(
                client, 
                "/topic/test3", 
                "Vibración detectada",  
                0,  // QoS
                0,  // Retain
                0   // Duplicado
            );
            ESP_LOGI(TAG, "Mensaje MQTT publicado por vibración");

            vTaskDelay(pdMS_TO_TICKS(1000)); 

            gpio_set_level(BUZZER_GPIO, 0);
            vibracion_detectada = false; 
        }
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }

   
}