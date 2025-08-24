#include "fortuna.h"

#include <stdlib.h>
#include <string.h>
#include <malloc/malloc.h>

#include "diskio.h"
#include "SDL3/SDL.h"

#include "usb/sdlhid.h"
#include "config/config.h"

static Event* event_queue = NULL;
static size_t event_queue_sz = 0;
static size_t event_queue_max_sz;


extern void disk_image_initialize();   // in diskio.c

void fortuna_init(uint16_t event_queue_size, void (*core1_step_function)(), int argc, char* argv[])
{
    config_load(argc, argv);

    event_queue = calloc(sizeof(Event), event_queue_size);
    event_queue_sz = 0;
    event_queue_max_sz = event_queue_size;

    disk_image_initialize();

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

        // user panel

        if (e.type == SDL_EVENT_KEY_DOWN && (e.key.mod & SDL_KMOD_CTRL) && (e.key.mod & SDL_KMOD_SHIFT)) {
            extern uint8_t panel_dipswitch[2];
            if (e.key.key == SDLK_F10) {
                panel_dipswitch[0] = !panel_dipswitch[0];
                Event ev = { .type = E_PANEL, .panel = { .button = PB_SWITCH_0, .new_value = panel_dipswitch[0] } };
                fortuna_add_event(&ev);
                continue;
            }
            if (e.key.key == SDLK_F11) {
                panel_dipswitch[1] = !panel_dipswitch[1];
                Event ev = { .type = E_PANEL, .panel = { .button = PB_SWITCH_1, .new_value = panel_dipswitch[1] } };
                fortuna_add_event(&ev);
                continue;
            }
            if (e.key.key == SDLK_F12) {
                Event ev = { .type = E_PANEL, .panel = { .button = PB_PUSH_BUTTON, .new_value = 1 } };
                fortuna_add_event(&ev);
                continue;
            }
        }

        // keyboard / mouse events

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

uint32_t fortuna_total_ram()
{
    return 640 * 1024;
}

uint32_t fortuna_free_ram()
{
    return 240 * 1024;
}