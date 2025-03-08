#ifndef MAIN_H
#define MAIN_H

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_netif.h"

#include "driver/i2c.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "WifiHandler.h"
#include "ledHandler.h"
#include "BuzzerHandler.h"
#include "mpuHandler.h"
#include "oledHandler.h"
#include "peripheralsList.h"

void app_main(void);

#endif // MAIN_H