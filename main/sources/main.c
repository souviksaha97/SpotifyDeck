#include <stdio.h>
#include <string.h>
#include "main.h"


#include "ssd1306.h"
#include "font8x8_basic.h"

#define tag "SSD1306"


SSD1306_t dev;
int center, top, bottom;
char lineChar[20];

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


  // i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, -1);
  // ssd1306_init(&dev, 128, 64);
  // ssd1306_clear_screen(&dev, false);
	// ssd1306_contrast(&dev, 0x00);
	
  // ESP_LOGI(tag, "SSD1306 Initialized");

  // top = 2;
	// center = 3;
	// bottom = 8;
	// ssd1306_display_text(&dev, 0, "SSD1306 128x64", 14, false);
	// ssd1306_display_text(&dev, 1, "ABCDEFGHIJKLMNOP", 16, false);
	// ssd1306_display_text(&dev, 2, "abcdefghijklmnop",16, false);
	// ssd1306_display_text(&dev, 3, "Hello World!!", 13, false);
	// //ssd1306_clear_line(&dev, 4, true);
	// //ssd1306_clear_line(&dev, 5, true);
	// //ssd1306_clear_line(&dev, 6, true);
	// //ssd1306_clear_line(&dev, 7, true);
	// ssd1306_display_text(&dev, 4, "SSD1306 128x64", 14, true);
	// ssd1306_display_text(&dev, 5, "ABCDEFGHIJKLMNOP", 16, true);
	// ssd1306_display_text(&dev, 6, "abcdefghijklmnop",16, true);
	// ssd1306_display_text(&dev, 7, "Hello World!!", 13, true);
 
  // ESP_LOGI(tag, "SSD1306 Displayed");
  // i2c_config_t conf;
  // conf.mode = I2C_MODE_MASTER;
  // conf.sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO;
  // conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
  // conf.scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO;
  // conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
  // conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
  // conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;

  // ESP_LOGI(tag, "I2C Configurations");
  // ESP_LOGI(tag, "I2C_MASTER_SDA_IO: %d", I2C_MASTER_SDA_IO);
  // ESP_LOGI(tag, "I2C_MASTER_SCL_IO: %d", I2C_MASTER_SCL_IO);
  // ESP_LOGI(tag, "I2C_MASTER_NUM: %d", I2C_MASTER_NUM);
  // ESP_LOGI(tag, "I2C_MASTER_FREQ_HZ: %d", I2C_MASTER_FREQ_HZ);
  // ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_NUM, &conf));
  // ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0));


  // ESP_LOGI(tag, "SSD1306 Completed");
  // Initialize Wi-Fi
  ESP_ERROR_CHECK((esp_err_t) wifi_init_sta());
}

void app_main(void)
{

  init();
  ESP_LOGI("MAIN", "Waiting for 5 seconds");

  vTaskDelay(pdMS_TO_TICKS(5000));

  // Start Ping Task
  xTaskCreate(&ping_task, "ping_task", 4096, NULL, 5, NULL);
  xTaskCreate(&http_get_task, "http_get_task", 8192, NULL, 5, NULL);
  xTaskCreate(&led_handler, "led_handler", 8192, NULL, 5, NULL);
  xTaskCreate(&buzzer_task, "buzzer_task", 8192, NULL, 5, NULL);
}
