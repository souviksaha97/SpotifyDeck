#include "BuzzerHandler.h"

QueueHandle_t buzzerQueue;

const piano_note_t tune[] = {
    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_E4, NOTE_G4, NOTE_B4, NOTE_A4,
    NOTE_A4, NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_E4,
    NOTE_F4, NOTE_A4, NOTE_C5, NOTE_B4, NOTE_B4, NOTE_F4, NOTE_A4, NOTE_G4, NOTE_F4,
    // Repeat or add more for a longer loop
};

Note bullet_fire = {
    .pitch = NOTE_G5,
    .loudness = 500,
    .loud_time = 0.05f,
    .no_loud_time = 0.01f,
    .loud_cycle_time = 1};

Note explosion_sound = {
    .pitch = NOTE_C3,
    .loudness = 800,
    .loud_time = 0.3f,
    .no_loud_time = 0.05f,
    .loud_cycle_time = 3};

Note score_point = {
    .pitch = NOTE_C6,      // A bright, positive sound
    .loudness = 600,       // Medium loudness
    .loud_time = 0.1f,     // Short and snappy
    .no_loud_time = 0.05f, // Brief rest
    .loud_cycle_time = 2   // Play it twice for a cheerful effect
};

Note game_start = {
    .pitch = NOTE_E5,     // Bright, energetic tone to signal the start
    .loudness = 500,      // Medium loudness
    .loud_time = 0.2f,    // Slightly longer to build anticipation
    .no_loud_time = 0.1f, // Short rest between notes
    .loud_cycle_time = 3  // Play three times for excitement
};

// Game Over Sound
Note game_over = {
    .pitch = NOTE_B4,     // Lower, somber pitch to convey the end
    .loudness = 400,      // Softer to signify defeat
    .loud_time = 0.3f,    // Slightly longer to give it a final feeling
    .no_loud_time = 0.2f, // Rest time to emphasize the "end"
    .loud_cycle_time = 2  // Play twice for emphasis
};

const float note_durations[] = {
    0.2, 0.2, 0.4, 0.1, 0.2, 0.2, 0.4, 0.4,
    0.1, 0.2, 0.2, 0.4, 0.1, 0.2, 0.2, 0.1,
    0.2, 0.2, 0.4, 0.4, 0.1, 0.2, 0.2, 0.2, 0.2};

static void configure_buzzer(void)
{
    buzzer_init(BUZZER_GPIO);
}

void buzzer_task(void *pvParameters)
{
    configure_buzzer();
    while (1)
    {
        const uint32_t loudness = 500;     // Adjust loudness (0-1023)
        const float no_loud_time = 0.05;   // Silence between notes
        const uint8_t loud_cycle_time = 1; // Single cycle per note

        while (true)
        { // Infinite loop for continuous background music
            Note note;
            for (int i = 0; i < sizeof(tune) / sizeof(tune[0]); i++)
            {
                // buzzer(tune[i], loudness, note_durations[i], no_loud_time, loud_cycle_time);
                if (xQueueReceive(buzzerQueue, &note, 0) == pdTRUE)
                {
                    buzzer(note.pitch, note.loudness, note.loud_time, note.no_loud_time, note.loud_cycle_time);
                }
                else
                {
                    // buzzer(tune[i], loudness, note_durations[i], no_loud_time, loud_cycle_time);
                }
            }
            vTaskDelay(100 / portTICK_PERIOD_MS); // Delay between loops
        }
    }
}