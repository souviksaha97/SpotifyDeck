#include <stdio.h>
#include "ssd1306.h"
#include "oledHandler.h"

typedef struct
{
    uint8_t x;
    uint8_t y;
} position_t;

uint32_t random_asteroid = 0;

static ssd1306_handle_t ssd1306_dev = NULL;
QueueHandle_t oledQueue;
SemaphoreHandle_t oledMutex;
position_t spaceship_pos;

position_t asteroid_pos[MAX_ASTERIODS];

static void oled_init()
{
    ssd1306_dev = ssd1306_create(I2C_MASTER_NUM, SSD1306_I2C_ADDRESS);
    ssd1306_refresh_gram(ssd1306_dev);
    ssd1306_clear_screen(ssd1306_dev, 0x00);

    spaceship_pos.x = SSD1306_WIDTH / 2 - spaceship_width / 2;
    spaceship_pos.y = SSD1306_HEIGHT - spaceship_height;

    for (int i = 0; i < MAX_ASTERIODS; i++)
    {
        asteroid_pos[i].x = UINT8_MAX;
        asteroid_pos[i].y = UINT8_MAX;
    }
}

void render_spaceship(char direction)
{
    // ssd1306_clear_screen(ssd1306_dev, 0x00);
    if (direction == 'R')
    {
        spaceship_pos.x += 3;
    }
    else if (direction == 'L')
    {
        spaceship_pos.x -= 3;
    }
    else
    {
        spaceship_pos.x = spaceship_pos.x;
    }

    if (spaceship_pos.x < spaceship_width / 2)
    {
        spaceship_pos.x = spaceship_width / 2;
    }
    else if (spaceship_pos.x > SSD1306_WIDTH - spaceship_width)
    {
        spaceship_pos.x = SSD1306_WIDTH - spaceship_width;
    }

    // ssd1306_draw_bitmap(ssd1306_dev, spaceship_pos.x, spaceship_pos.y, spaceship, spaceship_width, spaceship_height);
}

void render_asteroids()
{
    // ssd1306_clear_screen(ssd1306_dev, 0x00);
    char debug[100]; // Reduced buffer size, it's usually enough for the format
    for (int i = 0; i < MAX_ASTERIODS; i++)
    {
        // first check if there is an asteroid on the screen, if not run a random number and if the number is 0, then spawn an asteroid. if there are asteroids on the screen, then increment the y position of the asteroid. if they reach the bottom then reset the value to uint8_tMAX
        if (asteroid_pos[i].x == UINT8_MAX)
        {
            random_asteroid = esp_random() % 80;
            if (random_asteroid == 0)
            {
                // ESP_LOGI("OLED", "Asteroid spawned");
                asteroid_pos[i].x = esp_random() % (SSD1306_WIDTH - asteroid_width);
                asteroid_pos[i].y = asteroid_height;
            }
        }
        else
        {
            asteroid_pos[i].y += 1;
            if (asteroid_pos[i].y > SSD1306_HEIGHT)
            {
                asteroid_pos[i].x = UINT8_MAX;
                asteroid_pos[i].y = UINT8_MAX;
            }
        }
        // ssd1306_draw_bitmap(ssd1306_dev, asteroid_pos[i].x, asteroid_pos[i].y, asteroid, asteroid_width, asteroid_height);
        // snprintf(debug, sizeof(debug), "Asteroid %d: x: %d y: %d", i, asteroid_pos[i].x, asteroid_pos[i].y);
        // ESP_LOGI("OLED", "%s", debug);
    }
}

void refresh_screen()
{
    ssd1306_clear_screen(ssd1306_dev, 0x00);
    ssd1306_draw_bitmap(ssd1306_dev, spaceship_pos.x, spaceship_pos.y, spaceship, spaceship_width, spaceship_height);
    for (int i = 0; i < MAX_ASTERIODS; i++)
    {
        if (asteroid_pos[i].x != UINT8_MAX)
        {
            ssd1306_draw_bitmap(ssd1306_dev, asteroid_pos[i].x, asteroid_pos[i].y, asteroid, asteroid_width, asteroid_height);
        }
    }

    ssd1306_refresh_gram(ssd1306_dev);
}

void oled_task(void *pvParameters)
{

    oled_init();
    ESP_LOGI("OLED", "OLED task started");

    // ssd1306_draw_bitmap(ssd1306_dev, spaceship_pos.x, spaceship_pos.y, spaceship, spaceship_width, spaceship_height);
    // ssd1306_refresh_gram(ssd1306_dev);
    render_spaceship('N');
   
    char direction;

    while (1)
    {
        render_asteroids();
        if (xQueueReceive(oledQueue, &direction, portMAX_DELAY) == pdTRUE)
        {
            render_spaceship(direction);
            // ESP_LOGI("OLED", "Direction: %c | Position: %d", direction, spaceship_pos.x);
        }
        refresh_screen();
        vTaskDelay(pdMS_TO_TICKS(FRAME_TIME_MS));
        // Wait for data from the queue
    }
}
