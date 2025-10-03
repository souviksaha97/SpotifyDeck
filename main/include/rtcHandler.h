#ifndef RTC_HANDLER_H
#define RTC_HANDLER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "esp_log.h"
#include "esp_sntp.h"
#include "main.h"

void obtain_time(void);
void oled_time(void);

#endif // RTC_HANDLER_H