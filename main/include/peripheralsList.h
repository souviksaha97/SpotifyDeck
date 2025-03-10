#ifndef PERIPHERALS_LIST_H
#define PERIPHERALS_LIST_H

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "esp_err.h"
#include "ssd1306.h"
#include "mpu6050.h"

#define LED_GPIO 48

#define I2C_MASTER_SCL_IO 16        /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 17        /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_1    /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 100000   /*!< I2C master clock frequency */


extern QueueHandle_t oledQueue;
extern SemaphoreHandle_t oledMutex;


void i2c_master_init();
void queue_init();
#endif // PERIPHERALS_LIST_H