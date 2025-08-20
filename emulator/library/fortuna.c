#include "fortuna.h"

#include "raylib.h"

void fortuna_init(uint16_t event_queue_size, void (*core1_step_function)())
{
    vga_init();
    while (!WindowShouldClose())
        vga_step();
    CloseWindow();
}