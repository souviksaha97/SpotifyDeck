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
  queue_init();
  gpio_init();

  // Initialize Wi-Fi
  // send_to_oled((uint8_t *) "Wi-Fi initialized");
  // send_to_oled((uint8_t *) "Connected to AP");
}

void app_main(void)
{

  init();
  xTaskCreate(&mpu_task, "mpu_task", 2048, NULL, 5, NULL);
  xTaskCreate(&oled_task, "oled_task", 8192, NULL, 5, NULL);
  xTaskCreate(&button_task, "button_task", 1024, NULL, 5, NULL);
  xTaskCreate(&buzzer_task, "buzzer_task", 2048, NULL, 5, NULL);
  // send_to_oled((uint8_t *) "Tasks started");
}
