#include <stdio.h>
#include <string.h>
#include "main.h"


void init(void)
{
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  i2c_master_init();

  // Initialize Wi-Fi
  ESP_ERROR_CHECK((esp_err_t) wifi_init_sta());
}



void app_main(void)
{

  init();
  ESP_LOGI("MAIN", "Waiting for 5 seconds");

  vTaskDelay(pdMS_TO_TICKS(5000));

  // Start Ping Task
  // xTaskCreate(&ping_task, "ping_task", 4096, NULL, 5, NULL);
  // xTaskCreate(&http_get_task, "http_get_task", 8192, NULL, 5, NULL);
  // xTaskCreate(&led_handler, "led_handler", 8192, NULL, 5, NULL);
  // xTaskCreate(&buzzer_task, "buzzer_task", 8192, NULL, 5, NULL);
  // xTaskCreate(&mpu_task, "mpuTaskHandler", 8192, NULL, 5, NULL);
  xTaskCreate(&oled_task, "oledTaskHandler", 8192, NULL, 5, NULL);
}
