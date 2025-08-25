#include "audio.h"

#include <float.h>

#include "SDL3/SDL.h"

#define MIN(a, b) ((a)<(b)?(a):(b))

#define SAMPLE_RATE   44000
#define AMPLITUDE     32767

static bool audio_ok = false;

static SDL_AudioStream* stream;

static FMusic current_note = { PAUSE, 0 };

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

    current_note = *sound;
}

void audio_step()
{
    if (current_note.note == PAUSE)
        return;

    // samples per update
    size_t samples_per_update = (double) SAMPLE_RATE / ((double) current_note.note / 1000.f) / 4.f;

    // generate next 20ms of square wave
    const int minimum_audio = (SAMPLE_RATE * sizeof (int8_t)) / 2;
    if (SDL_GetAudioStreamQueued(stream) < minimum_audio) {
        static float samples[512];
        int i;

        bool swap = true;
        for (i = 0; i < SDL_arraysize(samples);) {
            memset(&samples[i], swap ? 100 : -100, MIN(samples_per_update, SDL_arraysize(samples) - i));
            i += samples_per_update;
        }

        SDL_PutAudioStreamData(stream, samples, sizeof (samples));
    }
}

void audio_set_music(FMusic const* sounds, size_t sz)
{
    if (!audio_ok)
        return;
}

void audio_play_music(bool nonstop)
{
    if (!audio_ok)
        return;
}

void audio_stop_music()
{
    if (!audio_ok)
        return;
}
