#ifndef _BUZZERHANDLER_H
#define _BUZZERHANDLER_H

#include "buzzer.h"
#include "esp_log.h"
#include "peripheralsList.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

typedef struct
{
    piano_note_t pitch;      // The note (e.g., NOTE_C4, NOTE_A5, etc.)
    uint32_t loudness;       // Loudness level (0 to max duty cycle)
    float loud_time;         // Duration for which the note is played (in seconds)
    float no_loud_time;      // Rest time after the note (in seconds)
    uint8_t loud_cycle_time; // Number of times to repeat the note
} Note;

extern Note bullet_fire;
extern Note explosion_sound;
extern Note score_point;
extern Note game_start;
extern Note game_over;

void buzzer_task(void *pvParameters);

#endif // _BUZZERHANDLER_H