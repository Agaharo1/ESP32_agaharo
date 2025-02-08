#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "ssd1306.h"
#include "font8x8_basic.h"
#include "fontDisplay.h"

#define tag "SSD1306"

#define IMAGES 10

void app_main(void)
{
	SSD1306_t dev;
	int  top;

#if CONFIG_I2C_INTERFACE
	ESP_LOGI(tag, "INTERFACE is i2c"); // Es una función de ESP-IDF utilizada para imprimir mensajes de información (I de "Info") en la consola serie.
	ESP_LOGI(tag, "CONFIG_SDA_GPIO=%d", CONFIG_SDA_GPIO);
	ESP_LOGI(tag, "CONFIG_SCL_GPIO=%d", CONFIG_SCL_GPIO);
	ESP_LOGI(tag, "CONFIG_RESET_GPIO=%d", CONFIG_RESET_GPIO);
	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
#endif // CONFIG_I2C_INTERFACE

#if CONFIG_FLIP // Este fragmento de código se encarga de voltear la pantalla SSD1306 verticalmente (180°) si la opción CONFIG_FLIP está habilitada en menuconfig.
	dev._flip = true;
	ESP_LOGW(tag, "Flip upside down");
#endif

#if CONFIG_SSD1306_128x64 // Este bloque de código se encarga de inicializar la pantalla SSD1306 si su resolución es 128x64 píxeles
	ESP_LOGI(tag, "Panel is 128x64");
	ssd1306_init(&dev, 128, 64);
#endif							  // CONFIG_SSD1306_128x64
	ssd1306_contrast(&dev, 0xff); // ajustar el contraste de la pantalla

	uint8_t *buffer = (uint8_t *)malloc(8 * 128); // 8 page 128 pixel
	if (buffer == NULL)
	{
		ESP_LOGE(tag, "malloc failed");
		while (1)
		{
			vTaskDelay(1);
		}
	}
	uint8_t *segmentImage = (uint8_t *)malloc(IMAGES * 8 * 32); // 10 image 8 page 32pixel
	if (segmentImage == NULL)
	{
		ESP_LOGE(tag, "malloc failed");
		while (1)
		{
			vTaskDelay(1);
		}
	}

	ssd1306_clear_screen(&dev, false);
	ssd1306_display_text(&dev, 0, " Celsius ~C", 12, false);
	ssd1306_hardware_scroll(&dev, SCROLL_RIGHT);
	ssd1306_display_text(&dev, 1, " Temp CasaValle", 16, false);

	// Convert from segmentDisplay to segmentImage
	for (int imageIndex = 0; imageIndex < IMAGES; imageIndex++)
	{
		// ssd1306_clear_screen(&dev, false);
		ssd1306_bitmaps(&dev, 0, 20, segmentDisplay[imageIndex], 32, 48, false);
		ssd1306_get_buffer(&dev, buffer);
		int segmentImageIndex = imageIndex * 256;
		for (int page = 0; page < 8; page++)
		{
			memcpy(&segmentImage[segmentImageIndex + page * 32], &buffer[page * 128], 32);
		}
	}

	for (int page = 2; page < 8; page++)
	{
		ssd1306_display_image(&dev, page, 32, &segmentImage[page * 32], 32);
		ssd1306_display_image(&dev, page, 64, &segmentImage[page * 32], 32);
	}
	uint8_t image[9];
	top = 7;											  // ALTURA
	memset(image, 0, sizeof(image));					  // Asignar memoria
	memcpy(image, font8x8_basic_tr[127], 8);			  // Asigar el caracter 127 a la memomria que es "."
	ssd1306_display_image(&dev, top, (6 * 10), image, 8); // Pintar el caracter que tiene anchura 8

	memset(image, 0, sizeof(image));					 // Asignar memoria
	memcpy(image, font8x8_basic_tr[111], 8);			 // Asigar el caracter 111 a la memomria que es "o"
	ssd1306_display_image(&dev, 3, (10 * 10), image, 8); // Pintar el caracter que tiene anchura 8


	int digit2 = 0;
	int digit3 = 0;

	while (1) //Bucle principal
	{
		ssd1306_hardware_scroll(&dev, SCROLL_STOP); //Paro el scroll sino da fallo
		int segmentImageIndex2 = digit2 * 256;
		for (int page = 3; page < 8; page++)
		{
			ssd1306_display_image(&dev, page, 30, &segmentImage[segmentImageIndex2 + page * 32], 32);
		}
		digit2++;
		if (digit2 == 10)
		{
			digit2 = 0;
		}
		int segmentImageIndex3 = digit3 * 256;
		for (int page = 3; page < 8; page++)
		{
			ssd1306_display_image(&dev, page, 66, &segmentImage[segmentImageIndex3 + page * 32], 32);
		}
		digit3++;
		if (digit3 == 10)
		{
			digit3 = 0;
		}
		ssd1306_hardware_scroll(&dev, SCROLL_RIGHT); //Reanudo el scroll	
		vTaskDelay(70);
	}

}