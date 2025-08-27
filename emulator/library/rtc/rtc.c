#include "rtc.h"

#include <time.h>

#include "SDL3/SDL.h"

time_t initial_time;

void rtc_init()
{
    time(&initial_time);
    initial_time += (SDL_GetTicks() / 1000);
}

DateTime rtc_get()
{
    time_t time = initial_time + (SDL_GetTicks() / 1000);
    struct tm* t = localtime(&time);
    return (DateTime) {
        .year = (uint16_t) (t->tm_year + 1900),
        .month = (uint8_t) (t->tm_mon + 1),
        .day = (uint8_t) t->tm_mday,
        .weekday = (uint8_t) t->tm_wday,
        .hours = (uint8_t) t->tm_hour,
        .minutes = (uint8_t) t->tm_min,
        .seconds = (uint8_t) t->tm_sec,
    };
}

void rtc_set(DateTime d)
{
    struct tm t = {
        .tm_year = d.year - 1900,
        .tm_mon = d.month - 1,
        .tm_mday = d.day,
        .tm_hour = d.hours - 1,
        .tm_min = d.minutes,
        .tm_sec = d.seconds,
    };
    initial_time = mktime(&t) - (SDL_GetTicks() / 1000);
}
