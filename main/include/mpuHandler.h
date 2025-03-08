#ifndef MPU_HANDLER_H
#define MPU_HANDLER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "driver/i2c.h"
#include "mpu6050.h"
#include "peripheralsList.h"
#include "esp_log.h"

#define TAG "MPU6050"

void mpu_task(void *pvParameters);

#endif // MPU_HANDLER_H