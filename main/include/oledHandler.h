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
#include <math.h>

#define MAX_MESSAGE_LENGTH 64

#define FPS 20
#define FRAME_TIME_MS (1000 / FPS)

#define MAX_ASTEROIDS 5
#define MAX_BULLETS 100

#define BASE_BUFFER 2     // Starting buffer at level 0
#define BUFFER_INCREMENT 1 // Buffer increment per level

void oled_task(void *pvParameters);
void send_to_oled(const uint8_t *msg);

#endif // OLED_HANDLER_H