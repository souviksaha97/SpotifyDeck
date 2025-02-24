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
#include "esp_http_client.h"

#define WIFI_SSID      "testWifi"       // Replace with your Wi-Fi SSID
#define WIFI_PASS      "12345678"       // Replace with your Wi-Fi Password
#define MAX_RETRY      5                // Maximum number of retries
#define PING_TARGET    "8.8.8.8"

#define WEB_SERVER "jsonplaceholder.typicode.com"
#define WEB_URL "https://jsonplaceholder.typicode.com/posts/1"

static const char *TAG = "wifi_station";
static int retry_num = 0;
static EventGroupHandle_t s_wifi_event_group;

static EventBits_t wifiEventBits;

// Event bits
#define WIFI_CONNECTED_BIT  BIT0
#define WIFI_FAIL_BIT       BIT1

typedef enum {
    WIFI_OK,
    WIFI_FAIL
} wifi_status_t;


wifi_status_t wifi_init_sta(void);
void ping_task(void *pvParameters);
void http_get_task(void *pvParameters);

#endif // WIFI_HANDLER_H