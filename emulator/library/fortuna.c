#include "fortuna.h"

#include "raylib.h"

void fortuna_init(uint16_t event_queue_size, void (*core1_step_function)())
{
    InitWindow(1280, 960, "fortuna-io-board emulator");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();
}