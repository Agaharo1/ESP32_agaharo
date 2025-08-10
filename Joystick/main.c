#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/gpio.h"

#define VRX_PIN ADC1_CHANNEL_6 // GPIO34
#define VRY_PIN ADC1_CHANNEL_7 // GPIO35
#define SW_PIN  18              // Botón

void app_main(void)
{
    // Configurar ADC
    adc1_config_width(ADC_WIDTH_BIT_12); // 0-4095
    adc1_config_channel_atten(VRX_PIN, ADC_ATTEN_DB_11); // 0-3.3V
    adc1_config_channel_atten(VRY_PIN, ADC_ATTEN_DB_11);

    // Configurar botón
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << SW_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    while (1) {
        int x = adc1_get_raw(VRX_PIN);
        int y = adc1_get_raw(VRY_PIN);
        int sw = gpio_get_level(SW_PIN);

        printf("X: %d | Y: %d | Botón: %s\n", x, y, sw == 0 ? "PULSADO" : "LIBRE");

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
