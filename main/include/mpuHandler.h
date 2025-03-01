#ifndef MPU_HANDLER_H
#define MPU_HANDLER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "driver/i2c.h"
#include "mpu6050.h"
#include "esp_log.h"

#define I2C_NUM 0
#define I2C_MASTER_SCL_IO 16   // Example SCL pin
#define I2C_MASTER_SDA_IO 17   // Example SDA pin

#define TAG "MPU6050"

void i2c_master_init();
void mpu_task(void *pvParameters);

#endif // MPU_HANDLER_H