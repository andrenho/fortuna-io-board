#include "vga.h"

#include <stddef.h>
#include "raylib.h"

#include "framebuffer.h"

static Framebuffer* fb = NULL;

void vga_init()
{
    fb = fb_new(640, 480);

    InitWindow(fb->w * 2, fb->h * 2, "fortuna-io-board emulator");
    SetTargetFPS(60);
}

void vga_set_mode(VgaMode mode)
{
}

void vga_step()
{
    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();
}

struct Framebuffer* vga_framebuffer()
{
    return fb;
}