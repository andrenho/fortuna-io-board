#include "fortuna.h"

#include <stdlib.h>
#include <string.h>

#include "SDL3/SDL.h"
#include "usb/sdlhid.h"

static Event* event_queue = NULL;
static size_t event_queue_sz = 0;
static size_t event_queue_max_sz;

void fortuna_init(uint16_t event_queue_size, void (*core1_step_function)())
{
    event_queue = calloc(sizeof(Event), event_queue_size);
    event_queue_sz = 0;
    event_queue_max_sz = event_queue_size;

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    vga_init();
}

static MouseButton mouse_buttons()
{
    Uint32 b = SDL_GetMouseState(NULL, NULL);
    uint8_t buttons = 0;
    if (b & SDL_BUTTON_LMASK)
        buttons |= MB_LEFT;
    if (b & SDL_BUTTON_MMASK)
        buttons |= MB_MIDDLE;
    if (b & SDL_BUTTON_RMASK)
        buttons |= MB_RIGHT;
    return buttons;
}

static void emulator_ui_events()
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_EVENT_QUIT:
                exit(0);
            case SDL_EVENT_KEY_UP:
            case SDL_EVENT_KEY_DOWN: {
                Event ev = {
                    .type = E_KEYBOARD,
                    .key = (KeyboardEvent) {
                        .hid_key = SDLKeycodeToHID(e.key.key),
                        .chr = e.key.key,
                        .ctrl = e.key.mod & SDL_KMOD_CTRL,
                        .alt = e.key.mod & SDL_KMOD_ALT,
                        .shift = e.key.mod & SDL_KMOD_SHIFT,
                        .pressed = e.key.down,
                    }
                };
                fortuna_add_event(&ev);
                break;
            }
            case SDL_EVENT_MOUSE_MOTION: {
                Event ev = {
                    .type = E_MOUSE,
                    .mouse = (MouseEvent) {
                        .x = e.motion.xrel,
                        .y = e.motion.yrel,
                        .wheel = 0,
                        .buttons = mouse_buttons(),
                    }
                };
                fortuna_add_event(&ev);
                break;
            }
            case SDL_EVENT_MOUSE_WHEEL: {
                Event ev = {
                    .type = E_MOUSE,
                    .mouse = (MouseEvent) {
                        .x = 0,
                        .y = 0,
                        .wheel = e.wheel.y,
                        .buttons = mouse_buttons(),
                    }
                };
                fortuna_add_event(&ev);
                break;
            }
            case SDL_EVENT_MOUSE_BUTTON_UP:
            case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                Event ev = {
                    .type = E_MOUSE,
                    .mouse = (MouseEvent) {
                        .x = 0,
                        .y = 0,
                        .wheel = 0,
                        .buttons = mouse_buttons(),
                    }
                };
                fortuna_add_event(&ev);
                break;
            }
            default:
                break;
        }
    }
}

bool fortuna_next_event(Event* event)
{
    // execute subsystems
    vga_step();

    // check SDL events
    emulator_ui_events();

    // return last queue event
    if (event_queue_sz == 0)
        return false;
    memcpy(event, &event_queue[0], sizeof(Event));
    memmove(&event_queue[0], &event_queue[1], sizeof(Event) * event_queue_sz - 1);
    --event_queue_sz;
    return true;
}

bool fortuna_add_event(Event const* event)
{
    if (event_queue_sz + 1 > event_queue_max_sz)
        return false;

    memcpy(&event_queue[event_queue_sz], event, sizeof(Event));
    ++event_queue_sz;
    return true;
}