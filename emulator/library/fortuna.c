#include "fortuna.h"

#include <stdlib.h>
#include <string.h>

#include "raylib.h"

static Event* event_queue = NULL;
static size_t event_queue_sz = 0;
static size_t event_queue_max_sz;

void fortuna_init(uint16_t event_queue_size, void (*core1_step_function)())
{
    event_queue = calloc(sizeof(Event), event_queue_size);
    event_queue_size = 0;
    event_queue_max_sz = event_queue_size;

    vga_init();
}

static void emulator_ui_events()
{
}

bool fortuna_next_event(Event* event)
{
    // do we need to close the application?
    if (WindowShouldClose()) {
        CloseWindow();
        exit(0);
    }

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