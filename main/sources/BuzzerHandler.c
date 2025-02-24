#include "BuzzerHandler.h"

#define TAG "BuzzerHandler"

static void configure_buzzer(void)
{
    ESP_LOGI(TAG, "Example configured to play a note on the buzzer!");
    buzzer_init(4);
}

void buzzer_task(void *pvParameters)
{
    configure_buzzer();
    while (1) 
    {   
        ESP_LOGI(TAG, "Playing a note on the buzzer!");
        buzzer(NOTE_G6, 7168, 1, 1, 5);
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
} 