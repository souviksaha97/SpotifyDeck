#include <stdio.h>
#include "ssd1306.h"
#include "oledHandler.h"

static ssd1306_handle_t ssd1306_dev = NULL;

static void oled_init()
{
    ssd1306_dev = ssd1306_create(I2C_MASTER_NUM, SSD1306_I2C_ADDRESS);
    ssd1306_refresh_gram(ssd1306_dev);
    ssd1306_clear_screen(ssd1306_dev, 0x00);
}

void oled_task(void *pvParameters)
{
    int counter = 0;
    oled_init();
    char data_str[20] = {0};
    while (1)
    {
        sprintf(data_str, "HELLO - %d", counter++);
        ssd1306_draw_string(ssd1306_dev, 0, 0, (const uint8_t *)data_str, 16, 1);
        ssd1306_refresh_gram(ssd1306_dev);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}