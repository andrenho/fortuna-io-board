#include "pico.h"

#include <stdlib.h>

static Uint32 custom_callback(void *userdata, SDL_TimerID timerID, Uint32 interval)
{
    repeating_timer_t* rt = (repeating_timer_t *) userdata;
    bool r = rt->callback(rt);
    return r ? interval : 0;
}

bool add_repeating_timer_ms(int32_t delay_ms, repeating_timer_callback_t callback, void* user_data, repeating_timer_t* out)
{
    out->delay_us = delay_ms * 1000;
    out->pool = NULL;
    out->alarm_id = SDL_AddTimer(delay_ms, custom_callback, out);
    out->callback = callback;
    out->user_data = user_data;
    return true;
}
