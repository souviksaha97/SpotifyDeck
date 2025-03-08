#ifndef OLED_HANDLER_H
#define OLED_HANDLER_H


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "driver/i2c.h"
#include "ssd1306.h"
#include "peripheralsList.h"
#include "esp_log.h"

#define TAG "OLED"

void oled_task(void *pvParameters);

#endif // OLED_HANDLER_H