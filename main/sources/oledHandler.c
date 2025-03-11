#include <stdio.h>
#include "ssd1306.h"
#include "oledHandler.h"

typedef struct
{
    uint8_t x;
    uint8_t y;
} position_t;

uint32_t random_asteroid = 0;

bool collision = false;

uint8_t score = 0;

uint8_t lives = 3;

static ssd1306_handle_t ssd1306_dev = NULL;
QueueHandle_t oledQueue;
SemaphoreHandle_t oledMutex;

position_t spaceship_pos;
position_t asteroid_pos[MAX_ASTEROIDS];
position_t explosion_pos;
position_t bullet_pos[MAX_BULLETS];

TickType_t collision_time = 0;

char debug[100]; // Reduced buffer size, it's usually enough for the format

static void oled_init()
{
    ssd1306_dev = ssd1306_create(I2C_MASTER_NUM, SSD1306_I2C_ADDRESS);
    ssd1306_refresh_gram(ssd1306_dev);
    ssd1306_clear_screen(ssd1306_dev, 0x00);

    spaceship_pos.x = SSD1306_WIDTH / 2 - spaceship_width / 2;
    spaceship_pos.y = SSD1306_HEIGHT - spaceship_height;

    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        asteroid_pos[i].x = UINT8_MAX;
        asteroid_pos[i].y = UINT8_MAX;
    }

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bullet_pos[i].x = UINT8_MAX;
        bullet_pos[i].y = UINT8_MAX;
    }

    collision_time = xTaskGetTickCount();
}

int get_asteroid_buffer(int game_level) {
    // Increase buffer as the level increases (linear growth)
    int buffer = BASE_BUFFER + (game_level * BUFFER_INCREMENT);
    if (buffer > 5)
    {
        buffer = 5;
    }
    return buffer;
}

int get_game_level(int score) {
    // Clamp score between 0 and 20
    if (score < 0) score = 0;
    if (score > 20) score = 20;

    // Use an exponential formula to calculate the level
    // The level will increase more quickly as the score approaches 20
    return 1 + (int)(pow(score / 20.0, 2) * 9); // Exponential scaling
}

bool will_spawn_asteroid(int game_level) {
    // We can base the probability on the game level.
    // Higher game levels have a higher chance of spawning an asteroid.
    
    // Normalize game level to range from 0 to 1 (with max level of 10)
    float normalized_level = game_level / 10.0;

    // We use the normalized game level to scale the chance of an asteroid appearing
    // Exponentially increasing chance with higher levels
    float spawn_chance = pow(normalized_level, 2);  // Exponential scaling
    
    // Generate a random number and compare it with the spawn chance
    // If the random number is less than spawn_chance, an asteroid will spawn
    return (esp_random() % 100) < (spawn_chance * 100);
}

void reset_asteroid_bullets(void)
{
    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        asteroid_pos[i].x = UINT8_MAX;
        asteroid_pos[i].y = UINT8_MAX;
    }

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        bullet_pos[i].x = UINT8_MAX;
        bullet_pos[i].y = UINT8_MAX;
    }
}

void render_game_over(void)
{
    ssd1306_clear_screen(ssd1306_dev, 0x00);
    ssd1306_draw_string(ssd1306_dev, SSD1306_WIDTH, SSD1306_HEIGHT - 5, (uint8_t *)"Game Over", 16, 1);
    ssd1306_refresh_gram(ssd1306_dev);

    reset_asteroid_bullets();

    collision = false;

    xQueueSend(buzzerQueue, &game_over, 0);
}

void render_bullets()
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullet_pos[i].x != UINT8_MAX)
        {
            bullet_pos[i].y -= 5;
            if (bullet_pos[i].y < 0 || bullet_pos[i].y > SSD1306_HEIGHT)
            {
                bullet_pos[i].x = UINT8_MAX;
                bullet_pos[i].y = UINT8_MAX;
            }
        }
    }
}

void render_score()
{
    char score_str[5];
    snprintf(score_str, sizeof(score_str), "%d", score);
    ssd1306_draw_string(ssd1306_dev, 0, 0, (uint8_t *)&score_str, 12, 1);
}

void render_explosion()
{
    explosion_pos.x = spaceship_pos.x - explosion_width / 2 + spaceship_width / 2;
    explosion_pos.y = spaceship_pos.y - explosion_height / 2 + spaceship_height / 2;
    xQueueSend(buzzerQueue, &explosion_sound, 0);
}

void check_collision(void)
{
    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (asteroid_pos[i].x != UINT8_MAX)
        {
            if (spaceship_pos.x < asteroid_pos[i].x + asteroid_width &&
                spaceship_pos.x + spaceship_width > asteroid_pos[i].x &&
                spaceship_pos.y < asteroid_pos[i].y + asteroid_height &&
                spaceship_pos.y + spaceship_height > asteroid_pos[i].y)
            {
                // ESP_LOGI("OLED", "Collision detected");
                collision = true;
                lives--;
                collision_time = xTaskGetTickCount();
                if (lives == 0)
                {
                    score = 0;
                    lives = 3;

                    render_game_over();
                    vTaskDelay(pdMS_TO_TICKS(5000));
                }

                reset_asteroid_bullets();
            }
        }
    }
    if (!collision)
    {
        sprintf(debug, "%ld", xTaskGetTickCount() - collision_time);
        ESP_LOGI("OLED", "%s", debug);
        sprintf(debug, "%ld", pdMS_TO_TICKS(600000) / get_game_level(score));
        ESP_LOGI("OLED", "%s", debug);
        sprintf(debug, "%d", get_game_level(score));
        ESP_LOGI("OLED", "%s", debug);
        if (xTaskGetTickCount() - collision_time > pdMS_TO_TICKS(600000) / get_game_level(score))
        {
            score++;
            collision_time = xTaskGetTickCount();
        }
        
    }

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullet_pos[i].x != UINT8_MAX)
        {
            for (int j = 0; j < MAX_ASTEROIDS; j++)
            {
                if (asteroid_pos[j].x != UINT8_MAX)
                {
                    if (bullet_pos[i].x < asteroid_pos[j].x + asteroid_width + get_asteroid_buffer(get_game_level(score)) &&
                        bullet_pos[i].x + bullet_width + get_asteroid_buffer(get_game_level(score)) > asteroid_pos[j].x &&
                        bullet_pos[i].y < asteroid_pos[j].y + asteroid_height + get_asteroid_buffer(get_game_level(score)) &&
                        bullet_pos[i].y + bullet_height + get_asteroid_buffer(get_game_level(score)) > asteroid_pos[j].y)
                    {
                        score++;
                        bullet_pos[i].x = UINT8_MAX;
                        bullet_pos[i].y = UINT8_MAX;
                        asteroid_pos[j].x = UINT8_MAX;
                        asteroid_pos[j].y = UINT8_MAX;
                        xQueueSend(buzzerQueue, &score_point, 0);
                    }
                }
            }
        }
    }
}

void render_heart(void)
{
    for (int i = 0; i < lives; i++)
    {
        ssd1306_draw_bitmap(ssd1306_dev, SSD1306_WIDTH - (i + 1) * heart_width - (i * 3), 0, heart, heart_width, heart_height);
    }
}

void render_spaceship(char direction)
{
    // ssd1306_clear_screen(ssd1306_dev, 0x00);
    if (direction == 'R')
    {
        spaceship_pos.x += get_game_level(score)/2 + 3;
    }
    else if (direction == 'L')
    {
        spaceship_pos.x -= get_game_level(score)/2 + 3;
        if (spaceship_pos.x > SSD1306_WIDTH - spaceship_width)
        {
            spaceship_pos.x = 0;
        }
    }
    else
    {
        spaceship_pos.x = spaceship_pos.x;
    }

    if (spaceship_pos.x < 0)
    {
        spaceship_pos.x = 0;
    }
    else if (spaceship_pos.x > SSD1306_WIDTH - spaceship_width)
    {
        spaceship_pos.x = SSD1306_WIDTH - spaceship_width;
    }
    // sprintf(debug, "Spaceship position: %d", spaceship_pos.x);
    ESP_LOGI("OLED", "%s", debug);
    // ssd1306_draw_bitmap(ssd1306_dev, spaceship_pos.x, spaceship_pos.y, spaceship, spaceship_width, spaceship_height);
}

void render_asteroids()
{
    // ssd1306_clear_screen(ssd1306_dev, 0x00);
    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        // first check if there is an asteroid on the screen, if not run a random number and if the number is 0, then spawn an asteroid. if there are asteroids on the screen, then increment the y position of the asteroid. if they reach the bottom then reset the value to uint8_tMAX
        if (asteroid_pos[i].x == UINT8_MAX)
        {
            if (will_spawn_asteroid(get_game_level(score)))
            {
                // ESP_LOGI("OLED", "Asteroid spawned");
                asteroid_pos[i].x = (spaceship_width / 2) + esp_random() % (SSD1306_WIDTH - spaceship_width);
                bool same_position = true;
                while (same_position)
                {
                    same_position = false;
                    for (int j = 0; j < MAX_ASTEROIDS; j++)
                    {
                        if (i != j && abs(asteroid_pos[j].x - asteroid_pos[i].x) < asteroid_width)
                        {
                            same_position = true;
                            asteroid_pos[i].x = esp_random() % (SSD1306_WIDTH - asteroid_width);
                            break;
                        }
                    }
                    // ESP_LOGI("OLED", "Reset asteroid");
                }

                asteroid_pos[i].y = asteroid_height;
            }
        }
        else
        {
            asteroid_pos[i].y += get_game_level(score)/2 + 1;
            if (asteroid_pos[i].y > SSD1306_HEIGHT)
            {
                asteroid_pos[i].x = UINT8_MAX;
                asteroid_pos[i].y = UINT8_MAX;
            }
        }
    }
}

void refresh_screen()
{
    ssd1306_clear_screen(ssd1306_dev, 0x00);
    ssd1306_draw_bitmap(ssd1306_dev, spaceship_pos.x, spaceship_pos.y, spaceship, spaceship_width, spaceship_height);
    for (int i = 0; i < MAX_ASTEROIDS; i++)
    {
        if (asteroid_pos[i].x != UINT8_MAX)
        {
            ssd1306_draw_bitmap(ssd1306_dev, asteroid_pos[i].x, asteroid_pos[i].y, asteroid, asteroid_width, asteroid_height);
        }
    }

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullet_pos[i].x != UINT8_MAX)
        {
            ssd1306_fill_point(ssd1306_dev, bullet_pos[i].x, bullet_pos[i].y, 1);
        }
    }

    if (collision)
    {
        ssd1306_draw_bitmap(ssd1306_dev, explosion_pos.x, explosion_pos.y, explosion, explosion_width, explosion_height);
    }
    render_score();
    render_heart();

    ssd1306_refresh_gram(ssd1306_dev);
}

void oled_task(void *pvParameters)
{

    oled_init();
    // ESP_LOGI("OLED", "OLED task started");

    // ssd1306_draw_bitmap(ssd1306_dev, spaceship_pos.x, spaceship_pos.y, spaceship, spaceship_width, spaceship_height);
    // ssd1306_refresh_gram(ssd1306_dev);
    render_spaceship('N');
    xQueueSend(buzzerQueue, &game_start, 0);

    vTaskDelay(pdMS_TO_TICKS(1000));

    char direction;

    while (1)
    {
        render_asteroids();
        if (xQueueReceive(oledQueue, &direction, portMAX_DELAY) == pdTRUE)
        {
            render_spaceship(direction);
            // ESP_LOGI("OLED", "Direction: %c | Position: %d", direction, spaceship_pos.x);
        }

        if (xQueueReceive(buttonQueue, &direction, 0) == pdTRUE)
        {
            // ESP_LOGI("OLED", "Button pressed");
            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (bullet_pos[i].x == UINT8_MAX)
                {
                    bullet_pos[i].x = spaceship_pos.x + spaceship_width / 2 - bullet_width / 2;
                    bullet_pos[i].y = spaceship_pos.y;
                    break;
                }
            }

            xQueueSend(buzzerQueue, &bullet_fire, 0);
        }
        render_bullets();
        check_collision();
        if (collision)
        {
            render_explosion();
        }
        refresh_screen();
        if (collision)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
            collision = false;
            spaceship_pos.x = SSD1306_WIDTH / 2 - spaceship_width / 2;
            spaceship_pos.y = SSD1306_HEIGHT - spaceship_height;
        }
        vTaskDelay(pdMS_TO_TICKS(FRAME_TIME_MS));
        // Wait for data from the queue
    }
}
