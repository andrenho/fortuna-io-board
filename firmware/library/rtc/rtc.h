#ifndef CLOCK_HH
#define CLOCK_HH

#include <stdint.h>

typedef struct DateTime {
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  weekday;
    uint8_t  hours;
    uint8_t  minutes;
    uint8_t  seconds;
} DateTime;

void     rtc_init();
DateTime rtc_get();
void     rtc_set(DateTime datetime);

#endif //CLOCK_HH
