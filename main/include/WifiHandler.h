#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_ping.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "ping/ping_sock.h"

#define WIFI_SSID      "testWifi"      // Replace with your Wi-Fi SSID
#define WIFI_PASS      "12345678"  // Replace with your Wi-Fi Password
#define MAX_RETRY      5                // Maximum number of retries

static const char *TAG = "wifi_station";
static int retry_num = 0;
static EventGroupHandle_t s_wifi_event_group;

// Event bits
#define WIFI_CONNECTED_BIT  BIT0
#define WIFI_FAIL_BIT       BIT1

void wifi_init_sta(void);
void ping_task(void *pvParameters);

#endif // WIFI_HANDLER_H