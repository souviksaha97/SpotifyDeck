#ifndef LED_HANDLER_H
#define LED_HANDLER_H

#include "peripheralsList.h"
#include "led_strip.h"
#include "mpuHandler.h"

#include "esp_log.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "stdint.h"

typedef struct {
    uint32_t r;
    uint32_t g;
    uint32_t b;
} led_data_t;

extern QueueHandle_t led_queue;

void led_handler(void *pvParameters);

#endif // LED_HANDLER_H
