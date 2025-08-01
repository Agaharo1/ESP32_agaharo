#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "driver/gpio.h"

#define TAG "MQ135_SENSOR"

// Pines usados
#define MQ135_ANALOG_PIN ADC1_CHANNEL_6  // GPIO34
#define MQ135_DIGITAL_PIN GPIO_NUM_27    // Opcional

void app_main(void)
{
    // Configuración del ADC
    adc1_config_width(ADC_WIDTH_BIT_12); // 0 - 4095
    adc1_config_channel_atten(MQ135_ANALOG_PIN, ADC_ATTEN_DB_11); // hasta ~3.3 V

    // Configuración de entrada digital (opcional)
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << MQ135_DIGITAL_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    while (1) {
        // Leer valor ADC
        int adc_raw = adc1_get_raw(MQ135_ANALOG_PIN);
        float voltage = (adc_raw / 4095.0) * 3.3;

        // Leer valor digital (opcional)
        int digital_state = gpio_get_level(MQ135_DIGITAL_PIN);

        ESP_LOGI(TAG, "ADC Raw: %d | Voltaje: %.2f V | D0: %s",
                 adc_raw, voltage, digital_state == 0 ? "ALTO nivel de gas" : "Normal");

        vTaskDelay(pdMS_TO_TICKS(1000)); // Esperar 1 segundo
    }
}
