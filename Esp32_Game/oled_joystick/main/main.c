#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "ssd1306.h"

#define VRX_PIN ADC1_CHANNEL_6 // GPIO34
#define VRY_PIN ADC1_CHANNEL_7 // GPIO35
#define SW_PIN  18              // Botón

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64

void app_main(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(VRX_PIN, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(VRY_PIN, ADC_ATTEN_DB_11);

    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << SW_PIN,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);

    SSD1306_t dev;
    i2c_master_init(&dev, 21, 22, 0);
    ssd1306_init(&dev, SCREEN_WIDTH, SCREEN_HEIGHT);
    ssd1306_clear_screen(&dev, false);

    // Posición en caracteres (cada carácter ocupa ~6-8 píxeles en horizontal, 1 página (8 píxeles) en vertical)
    // Para simplificar, usaremos pasos de 8 píxeles horizontalmente
    int char_x = SCREEN_WIDTH / 2;
    int char_y = SCREEN_HEIGHT / 2 / 8;  // página vertical

    int prev_char_x = char_x;
    int prev_char_y = char_y;

    while (1) {
        int raw_x = adc1_get_raw(VRX_PIN);
        int raw_y = adc1_get_raw(VRY_PIN);
        int sw = gpio_get_level(SW_PIN);

        // Actualizar posición
        if (raw_x > 1500) char_x -= 8;  // mover 8 píxeles a la izquierda
        if (raw_x < 2500) char_x += 8;  // mover 8 píxeles a la derecha
        if (raw_y < 1500) char_y += 1;  // mover página abajo
        if (raw_y > 2500) char_y -= 1;  // mover página arriba

        // Limitar bordes
        if (char_x < 0) char_x = 0;
        if (char_x > SCREEN_WIDTH - 8) char_x = SCREEN_WIDTH - 8;  // dejar espacio para un carácter
        if (char_y < 0) char_y = 0;
        if (char_y > (SCREEN_HEIGHT / 8) - 1) char_y = (SCREEN_HEIGHT / 8) - 1;

     if (char_x != prev_char_x || char_y != prev_char_y) {
    // Borra toda la línea anterior
    ssd1306_clear_line(&dev, prev_char_y, false);

    // Dibuja el nuevo carácter
    ssd1306_display_text_box1(&dev, char_y, char_x, "o", 1, 1, false, 0);

    ssd1306_show_buffer(&dev);

    prev_char_x = char_x;
    prev_char_y = char_y;
}

        if (sw == 0) {
            ssd1306_display_text_box1(&dev, char_y, char_x, " ", 1, 1, false, 0);
            char_x = SCREEN_WIDTH / 2;
            char_y = SCREEN_HEIGHT / 2 / 8;
            ssd1306_display_text_box1(&dev, char_y, char_x, "o", 1, 1, false, 0);
            ssd1306_show_buffer(&dev);

            prev_char_x = char_x;
            prev_char_y = char_y;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
