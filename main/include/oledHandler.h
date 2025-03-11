#ifndef OLED_HANDLER_H
#define OLED_HANDLER_H


#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "driver/i2c.h"
#include "ssd1306.h"
#include "peripheralsList.h"
#include "esp_log.h"
#include "sprites.h"
#include "main.h"
#include "esp_random.h"
#include "BuzzerHandler.h"

#define MAX_MESSAGE_LENGTH 64

#define FPS 20
#define FRAME_TIME_MS (1000 / FPS)

#define MAX_ASTERIODS 5
#define MAX_BULLETS 100

void oled_task(void *pvParameters);
void send_to_oled(const uint8_t *msg);

#endif // OLED_HANDLER_H