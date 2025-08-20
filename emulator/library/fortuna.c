#include "fortuna.h"

#include <stdlib.h>

#include "raylib.h"

void fortuna_init(uint16_t event_queue_size, void (*core1_step_function)())
{
    vga_init();
}

bool fortuna_next_event(Event* event)
{
    if (WindowShouldClose()) {
        CloseWindow();
        exit(0);
    }
    vga_step();

    // TODO ...

    return false;
}
