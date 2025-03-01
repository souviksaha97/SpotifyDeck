#include "mpuHandler.h"

static mpu6050_handle_t mpu6050 = NULL;
mpu6050_acce_value_t acce;
mpu6050_gyro_value_t gyro;
mpu6050_temp_value_t temp;

static void mpu6050_init()
{
    i2c_master_init();
    mpu6050 = mpu6050_create(I2C_NUM, MPU6050_I2C_ADDRESS);
    mpu6050_config(mpu6050, ACCE_FS_4G, GYRO_FS_500DPS);
    mpu6050_wake_up(mpu6050);
}

void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM, conf.mode, 0, 0, 0));
}

void mpu_task(void *pvParameters)
{
    mpu6050_init();
    while (1)
    {
        mpu6050_get_acce(mpu6050, &acce);
        mpu6050_get_gyro(mpu6050, &gyro);
        ESP_LOGI(TAG, "Accel: x=%0.2f, y=%0.2f, z=%0.2f", acce.acce_x, acce.acce_y, acce.acce_x);
        // ESP_LOGI(TAG, "Gyro: x=%0.2f, y=%0.2f, z=%0.2f", gyro.gyro_x, gyro.gyro_y, gyro.gyro_z);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}