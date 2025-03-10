#include "rtcHandler.h"

void obtain_time() {
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;

    while (timeinfo.tm_year < (2023 - 1900) && ++retry < retry_count) {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    if (timeinfo.tm_year >= (2023 - 1900)) {
        printf("Time set: %s", asctime(&timeinfo));
    } else {
        printf("Failed to set time.\n");
    }

    // Set local timezone (CET/CEST for Germany)
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();
    time(&now);
    localtime_r(&now, &timeinfo);

    printf("Local Time: %s", asctime(&timeinfo));
}

void oled_time(void)
{
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    char timeMessage[50];
    snprintf((char *)timeMessage, sizeof(timeMessage), "%s", asctime(&timeinfo));
    send_to_oled((uint8_t *)timeMessage);
}