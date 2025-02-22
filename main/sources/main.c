#include <stdio.h>
#include <string.h>
#include "main.h"

void app_main(void)
{
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Initialize Wi-Fi
  wifi_init_sta();

  vTaskDelay(pdMS_TO_TICKS(5000));

  // Start Ping Task
  xTaskCreate(ping_task, "ping_task", 4096, NULL, 5, NULL);
}
