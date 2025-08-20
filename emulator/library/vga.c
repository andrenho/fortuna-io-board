#include "vga.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#include "framebuffer.h"

static Framebuffer* fb = NULL;

static Color*    fb32 = NULL;
static Texture2D texture;

static Color palette[] = {
    { 0x00, 0x00, 0x00, 0xff },  // black
    { 0x00, 0x00, 0x80, 0xff },  // navy blue
    { 0x00, 0x64, 0x00, 0xff },  // dark green
    { 0x1e, 0x09, 0xff, 0xff },  // blue
    { 0x00, 0x80, 0x00, 0xff },  // green
    { 0x87, 0xce, 0xeb, 0xff },  // sky blue
    { 0x00, 0xff, 0x00, 0xff },  // lime
    { 0x00, 0xff, 0xff, 0xff },  // cyan
    { 0xff, 0x00, 0x00, 0xff },  // red
    { 0xff, 0x00, 0xff, 0xff },  // magenta
    { 0xff, 0x8c, 0x00, 0xff },  // dark orange
    { 0xee, 0x82, 0xee, 0xff },  // violet
    { 0xff, 0xa5, 0x00, 0xff },  // orange
    { 0xff, 0xc0, 0xcb, 0xff },  // pink
    { 0xff, 0xff, 0x00, 0xff },  // yellow
    { 0xe0, 0xe0, 0xe0, 0xff },  // white
};

void vga_init()
{
    InitWindow(640 * 2, 480 * 2, "fortuna-io-board emulator");
    SetTargetFPS(60);

    vga_set_mode(V_640x480);
}

static void convert_framebuffer_to_32()
{
    for (size_t i = 0, j = 0; i < (fb->w * fb->h); ++i) {
        fb32[j++] = palette[fb->data[i] & 0xf];
        fb32[j++] = palette[fb->data[i] >> 4];
    }
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
    fb32 = calloc(sizeof(Color), w * h);
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

    Rectangle src = { 0.f, 0.f, fb->w, fb->h };
    Rectangle dest = { 0.f, 0.f, GetScreenWidth(), GetScreenHeight() };

    DrawTexturePro(texture, src, dest, (Vector2) { 1, 1 }, 0.f, WHITE);

    EndDrawing();
}

struct Framebuffer* vga_framebuffer()
{
    return fb;
}