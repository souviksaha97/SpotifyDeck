#include "peripheralsList.h"
#include "driver/i2c.h"

void i2c_master_init()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_1, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_1, conf.mode, 0, 0, 0));
}

void queue_init()
{
    oledQueue = xQueueCreate(1, sizeof(char));
    oledMutex = xSemaphoreCreateMutex();

    buttonQueue = xQueueCreate(1, sizeof(int));

    buzzerQueue = xQueueCreate(5, sizeof(Note));
}

void gpio_init()
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_ENABLE,
        .intr_type = GPIO_INTR_DISABLE};
    gpio_config(&io_conf);
}