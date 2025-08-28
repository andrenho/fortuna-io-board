#ifndef PICO_TIME_H_
#define PICO_TIME_H_

#include "pico.h"

#include <SDL3/SDL_timer.h>

typedef SDL_TimerID alarm_id_t;

struct repeating_timer;

typedef bool (*repeating_timer_callback_t)(struct repeating_timer*);

struct repeating_timer {
    int64_t                    delay_us;
    void*                      pool;
    alarm_id_t                 alarm_id;
    repeating_timer_callback_t callback;
    void*                      user_data;
};

typedef struct repeating_timer repeating_timer_t;

bool add_repeating_timer_ms(int32_t delay_ms, repeating_timer_callback_t callback, void *user_data, repeating_timer_t *out);


#endif