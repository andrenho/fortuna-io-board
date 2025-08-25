#include "audio.h"

#include <float.h>
#include <stdlib.h>

#include "SDL3/SDL.h"

#define MIN(a, b) ((a)<(b)?(a):(b))

#define SAMPLE_RATE   44000
#define AMPLITUDE     50

static bool audio_ok = false;
static FMusic* music = nullptr;
static size_t  music_sz = 0;
static size_t  current_note = 0;
static bool    music_nonstop = false;
static bool    music_playing = false;

static SDL_AudioStream* stream;

void audio_init()
{
    SDL_AudioSpec spec = {
        .freq = SAMPLE_RATE,
        .format = SDL_AUDIO_S8,
        .channels = 1,
    };

    stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (stream != NULL) {
        audio_ok = true;
        SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(stream));
    }
}

void audio_play_single_note(FMusic const* sound)
{
    if (!audio_ok)
        return;

    size_t n_samples = (double) SAMPLE_RATE * ((double) sound->time / 1000.f);
    int8_t samples[n_samples];

    if (sound->note == PAUSE) {
        memset(samples, 0, n_samples);

    } else {
        size_t half_period_samples = (double) SAMPLE_RATE / ((double) sound->note / 1000.f) / 4.f;

        // generate wave for next note
        bool swap = true;
        for (size_t i = 0; i < SDL_arraysize(samples);) {
            memset(&samples[i], swap ? AMPLITUDE : -AMPLITUDE, MIN(half_period_samples, SDL_arraysize(samples) - i));
            swap = !swap;
            i += half_period_samples;
        }
    }
    SDL_PutAudioStreamData(stream, samples, sizeof (samples));
}

void audio_set_music(FMusic const* sounds, size_t sz)
{
    if (!audio_ok)
        return;

    music = (FMusic *) realloc(music, sz * sizeof(FMusic));
    memcpy(music, sounds, sz * sizeof(FMusic));
    music_sz = sz;
}

static Uint32 play_next_note(void *userdata, SDL_TimerID timerID, Uint32 interval)
{
    if (current_note >= music_sz) {
        if (music_nonstop) {
            current_note = 0;
        } else {
            music_playing = false;
            return 0;   // disable timer
        }
    }

    audio_play_single_note(&music[current_note]);

    ++current_note;

    return music[current_note].time;  // prepare to play next note
}

void audio_play_music(bool nonstop)
{
    if (!audio_ok)
        return;

    music_nonstop = nonstop;
    current_note = 0;
    music_playing = true;
    SDL_AddTimer(1, play_next_note, NULL);
}

void audio_stop_music()
{
    if (!audio_ok)
        return;

    music_playing = false;
    current_note = 0;
}