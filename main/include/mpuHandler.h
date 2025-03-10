#ifndef MPU_HANDLER_H
#define MPU_HANDLER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "driver/i2c.h"
#include "mpu6050.h"
#include "peripheralsList.h"
#include "esp_log.h"
#include "main.h"
#include "oledHandler.h"

#define MPU_QUEUE_SIZE 10

void mpu_task(void *pvParameters);

#endif // MPU_HANDLER_H