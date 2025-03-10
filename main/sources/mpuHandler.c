#include "mpuHandler.h"

static mpu6050_handle_t mpu6050 = NULL;
mpu6050_acce_value_t acce;
mpu6050_gyro_value_t gyro;
mpu6050_temp_value_t temp;

static void mpu6050_init()
{
    // i2c_master_init();
    mpu6050 = mpu6050_create(I2C_MASTER_NUM, MPU6050_I2C_ADDRESS);
    mpu6050_config(mpu6050, ACCE_FS_2G, GYRO_FS_500DPS);
    mpu6050_wake_up(mpu6050);
}

void convert_to_left_right(char *mpuMessage, mpu6050_acce_value_t acce)
{
    if (acce.acce_y > 0.1)
    {
        sprintf(mpuMessage, "L");
    }
    else if (acce.acce_y < -0.1)
    {
        sprintf(mpuMessage, "R");
    }
    else
    {
        sprintf(mpuMessage, "N");
    }
}

void mpu_task(void *arg) {
    mpu6050_init();

    while (1) {
        // Read raw data from MPU6050
        mpu6050_get_acce(mpu6050, &acce);

        // Send data to queue

        char mpuDirection;
        convert_to_left_right(&mpuDirection, acce);
        // ESP_LOGI("MPU", "Direction: %c Accel y:%0.2f", mpuDirection, acce.acce_y);

        xQueueSend(oledQueue, &mpuDirection, 0);

        vTaskDelay(pdMS_TO_TICKS(FRAME_TIME_MS));
    }
}

