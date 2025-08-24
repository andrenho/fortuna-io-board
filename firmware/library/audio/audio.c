#include "audio.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <pico/util/queue.h>

#define AUDIO_PIN 7

static uint slice_no;
static uint channel;

static FMusic* music = NULL;
static size_t music_sz = 0;
static size_t current_note = 0;
static bool   music_nonstop = false;
static bool   music_playing = false;

static alarm_pool_t* alarm_pool;

static void pwm_top(float desired_frequency, uint16_t divider, uint16_t* top, uint16_t* new_divider)
{
    *top = SYS_CLK_HZ / desired_frequency / divider;
    if (*top > 65535) {
        pwm_top(desired_frequency, divider / 2, top, new_divider);
    } else {
        *new_divider = divider;
    }
}

void audio_init()
{
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);
    slice_no = pwm_gpio_to_slice_num(AUDIO_PIN);
    channel = pwm_gpio_to_channel(AUDIO_PIN);
    alarm_pool = alarm_pool_create_with_unused_hardware_alarm(2);   // allows running alarm on core 1
}

void audio_set_music(FMusic const* sounds, size_t sz)
{
    music = (FMusic *) realloc(music, sz * sizeof(FMusic));
    memcpy(music, sounds, sz * sizeof(FMusic));
    music_sz = sz;
}

static void audio_play_sound(FMusic const* sound)
{
    if (sound->note == PAUSE) {
        pwm_set_enabled(slice_no, false);
    } else {
        uint16_t top, divider;
        pwm_top((float) sound->note / 1000.f, 128, &top, &divider);
        pwm_set_clkdiv(slice_no, divider);
        pwm_set_wrap(slice_no, top);
        pwm_set_chan_level(slice_no, channel, top / 2);
        pwm_set_enabled(slice_no, true);
        printf("X\n");
    }
}

static int64_t audio_play_next_note(alarm_id_t id, void *user_data)
{
    if (!music_playing) {
        pwm_set_enabled(slice_no, false);
        return 0;
    }

    if (current_note >= music_sz) {
        if (music_nonstop) {
            current_note = 0;
        } else {
            pwm_set_enabled(slice_no, false);
            music_playing = false;
            return 0;
        }
    }

    audio_play_sound(&music[current_note]);

    ++current_note;
    if (id != -1)
        alarm_pool_cancel_alarm(alarm_pool, id);
    alarm_pool_add_alarm_in_ms(alarm_pool, music[current_note].time, audio_play_next_note, NULL, true);
    return 0;
}

static int64_t alarm_stop_sound(alarm_id_t, void*)
{
    pwm_set_enabled(slice_no, false);
    return 0;
}

void audio_play_single_note(FMusic const* sound)
{
    if (!music_playing) {
        audio_play_sound(sound);
        alarm_pool_add_alarm_in_ms(alarm_pool, sound->time, alarm_stop_sound, NULL, true);
    }
}

void audio_play_music(bool nonstop)
{
    music_nonstop = nonstop;
    current_note = 0;
    music_playing = true;
    audio_play_next_note(-1, NULL);
}

void audio_stop_music()
{
    music_playing = false;
    current_note = 0;
}
