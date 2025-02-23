#include "WifiHandler.h"
#include "esp_wifi.h"
#include "cJSON.h"

#define MAX_HTTP_OUTPUT_BUFFER 2048  // Adjust size as needed

static char *response_buffer = NULL;  // Global or static buffer
static int response_length = 0;

// Event handler for Wi-Fi events
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (retry_num < MAX_RETRY)
        {
            esp_wifi_connect();
            retry_num++;
            ESP_LOGI(TAG, "Retrying to connect to the Wi-Fi...");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "Failed to connect to the Wi-Fi");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

wifi_status_t wifi_init_sta(void)
{
    // Create event group
    s_wifi_event_group = xEventGroupCreate();

    // Initialize TCP/IP stack
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Create default Wi-Fi station
    esp_netif_create_default_wifi_sta();

    // Initialize Wi-Fi with default config
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Register event handlers
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip);

    // Configure Wi-Fi station
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi initialization finished");

    // Wait for connection or failure
    wifiEventBits = xEventGroupWaitBits(s_wifi_event_group,
                                        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                        pdFALSE,
                                        pdFALSE,
                                        portMAX_DELAY);

    if (wifiEventBits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "Connected to SSID: %s", WIFI_SSID);
    }
    else if (wifiEventBits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to SSID: %s", WIFI_SSID);
        return WIFI_FAIL;
    }
    else
    {
        ESP_LOGE(TAG, "Unexpected event");
        return WIFI_FAIL;
    }

    // Cleanup event handlers
    esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id);
    esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip);
    vEventGroupDelete(s_wifi_event_group);

    return WIFI_OK;
}

// Success callback
static void ping_success_cb(esp_ping_handle_t hdl, void *args)
{
    uint8_t ttl;
    uint16_t seqno;
    uint32_t elapsed_time, recv_len;

    esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
    esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
    esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));

    ESP_LOGI("PING", "Ping success: seqno=%u time=%lums ttl=%u size=%lu", seqno, elapsed_time, ttl, recv_len);
}

// Ping end callback
static void ping_end_cb(esp_ping_handle_t hdl, void *args)
{
    ESP_LOGI("PING", "Ping session ended");
    esp_ping_stop(hdl);
    esp_ping_delete_session(hdl);
    vTaskDelete(NULL); // Delete the ping task
}

void ping_task(void *pvParameters)
{
    uint8_t waitForWifi = 0;
    while (waitForWifi < 5)
    {
        if (wifiEventBits & WIFI_CONNECTED_BIT)
        {
            ESP_LOGI(TAG, "Starting ping task");
            break;
        }
        waitForWifi++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    if (waitForWifi == 5)
    {
        ESP_LOGI(TAG, "Failed to connect to the Wi-Fi. Exiting ping task");
        vTaskDelete(NULL);
    }
    esp_ping_config_t ping_config = ESP_PING_DEFAULT_CONFIG();
    ip_addr_t target_addr;
    ipaddr_aton(PING_TARGET, &target_addr);
    ping_config.target_addr = target_addr;

    esp_ping_callbacks_t cbs = {
        .on_ping_success = ping_success_cb,
        .on_ping_end = ping_end_cb};

    esp_ping_handle_t ping;
    esp_ping_new_session(&ping_config, &cbs, &ping);
    esp_ping_start(ping);

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}



esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
        case HTTP_EVENT_ON_DATA:
            if (!esp_http_client_is_chunked_response(evt->client))
            {
                // Allocate memory on first chunk
                if (response_buffer == NULL)
                {
                    response_buffer = malloc(MAX_HTTP_OUTPUT_BUFFER);
                    if (response_buffer == NULL)
                    {
                        ESP_LOGE(TAG, "Failed to allocate memory for response buffer");
                        return ESP_FAIL;
                    }
                    response_length = 0;
                }

                // Accumulate chunks
                if (response_length + evt->data_len < MAX_HTTP_OUTPUT_BUFFER)
                {
                    memcpy(response_buffer + response_length, evt->data, evt->data_len);
                    response_length += evt->data_len;
                    response_buffer[response_length] = '\0';  // Null-terminate
                }
                else
                {
                    ESP_LOGE(TAG, "Response buffer overflow");
                    free(response_buffer);
                    response_buffer = NULL;
                    return ESP_FAIL;
                }
            }
            break;

        case HTTP_EVENT_ON_FINISH:
            if (response_buffer != NULL)
            {
                ESP_LOGI(TAG, "Complete JSON: %s", response_buffer);

                // Parse complete JSON
                cJSON *json = cJSON_Parse(response_buffer);
                if (json == NULL)
                {
                    const char *error_ptr = cJSON_GetErrorPtr();
                    if (error_ptr != NULL)
                    {
                        ESP_LOGE(TAG, "JSON Parsing Error before: %s", error_ptr);
                    }
                    free(response_buffer);
                    response_buffer = NULL;
                    return ESP_FAIL;
                }

                // Extract and log fields
                const cJSON *title = cJSON_GetObjectItemCaseSensitive(json, "title");
                if (cJSON_IsString(title) && title->valuestring != NULL)
                {
                    ESP_LOGI(TAG, "Title: %s", title->valuestring);
                }

                const cJSON *body = cJSON_GetObjectItemCaseSensitive(json, "body");
                if (cJSON_IsString(body) && body->valuestring != NULL)
                {
                    ESP_LOGI(TAG, "Body: %s", body->valuestring);
                }

                // Cleanup
                cJSON_Delete(json);
                free(response_buffer);
                response_buffer = NULL;
            }
            break;

        case HTTP_EVENT_DISCONNECTED:
            if (response_buffer != NULL)
            {
                free(response_buffer);
                response_buffer = NULL;
                response_length = 0;
            }
            break;

        default:
            break;
    }

    return ESP_OK;
}




void http_get_task(void *pvParameters)
{
    esp_http_client_config_t config = {
        .url = WEB_URL,
        .event_handler = _http_event_handler,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Perform the GET request
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP GET Status = %d, Content Length = %lld",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    }
    else
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    vTaskDelete(NULL); // Delete the task after completion
}