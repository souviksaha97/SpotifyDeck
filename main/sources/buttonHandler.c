#include "buttonHandler.h"

QueueHandle_t buttonQueue;

void button_task(void *pvParameters)
{
    while (1)
    {
        int button_state = gpio_get_level(BUTTON_GPIO);
        if (button_state == 1)
        { // Button is pressed
            xQueueSend(buttonQueue, &button_state, portMAX_DELAY);
        }
        vTaskDelay(pdMS_TO_TICKS(200)); // Debounce delay
    }
}