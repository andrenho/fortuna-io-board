#include "vga.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#include "framebuffer.h"

static Framebuffer* fb = NULL;

static uint8_t*  fb32 = NULL;
static Texture2D texture;

void vga_init()
{
    InitWindow(640 * 2, 480 * 2, "fortuna-io-board emulator");
    SetTargetFPS(60);

    vga_set_mode(V_640x480);
}

static void convert_framebuffer_to_32()
{
    memset(fb32, 0xff, fb->w * fb->h * 4);  // TODO
}

void vga_set_mode(VgaMode mode)
{
    uint16_t w = 0, h = 0;
    switch (mode) {
        case V_640x480: w = 640; h = 480; break;
        case V_640x240: w = 640; h = 240; break;
        case V_320x240: w = 320; h = 240; break;
    }

    if (IsTextureValid(texture))
        UnloadTexture(texture);
    fb_delete(fb);
    free(fb32);

    fb = fb_new(w, h);  // actual framebuffer
    fb32 = calloc(4, w * h);
    convert_framebuffer_to_32();

    Image image = {
        .data = fb32,
        .width = w,
        .height = h,
        .mipmaps = 1,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
    };

    texture = LoadTextureFromImage(image);
}

void vga_step()
{
    static char title[255];
    int fps = GetFPS();
    snprintf(title, sizeof title, "fortuna-io-board emulator - FPS %d", fps);
    SetWindowTitle(title);

    BeginDrawing();
    ClearBackground(BLACK);

    convert_framebuffer_to_32();
    UpdateTexture(texture, fb32);

    DrawTexture(texture, 0, 0, WHITE);

    EndDrawing();
}

struct Framebuffer* vga_framebuffer()
{
    return fb;
}