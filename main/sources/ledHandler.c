#include "ledHandler.h"

static uint8_t s_led_state = 0;

static led_strip_handle_t led_strip;

char *TAG = "LED_HANDLER";

static void blink_led(void)
{
    /* If the addressable LED is enabled */
    if (s_led_state) {
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        ESP_LOGI(TAG, "Blinking LED: ON");
        led_strip_set_pixel(led_strip, 0, 16, 16, 16);
        /* Refresh the strip to send data */
        led_strip_refresh(led_strip);

        s_led_state = 0;
    } else {
        ESP_LOGI(TAG, "Blinking LED: OFF");
        /* Set all LED off to clear all pixels */
        led_strip_clear(led_strip);

        s_led_state = 1;
    }
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_GPIO,
        .max_leds = 1, // at least one LED on board
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}

void led_handler(void *pvParameters)
{
    configure_led();
    while (1) {
        blink_led();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}