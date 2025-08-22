#ifndef SDLHID_H_
#define SDLHID_H_

#include <SDL3/SDL.h>
#include <stdint.h>

uint16_t SDLKeycodeToHID(SDL_Keycode key);

#endif