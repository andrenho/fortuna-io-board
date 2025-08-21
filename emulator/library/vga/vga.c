#include "vga.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "framebuffer.h"

#include "SDL3/SDL.h"

static Framebuffer* fb = NULL;

static SDL_Window*   window = NULL;
static SDL_Renderer* ren = NULL;
static SDL_Color*    fb32 = NULL;
static SDL_Texture*  texture = NULL;

static SDL_Color palette[] = {
    { 0x00, 0x00, 0x00, 0xff },  // black
    { 0x00, 0x00, 0xe0, 0xff },  // navy blue
    { 0x00, 0x64, 0x00, 0xff },  // dark green
    { 0x00, 0x64, 0xe0, 0xff },  // blue
    { 0x00, 0x94, 0x00, 0xff },  // green
    { 0x64, 0x94, 0xe0, 0xff },  // sky blue
    { 0x00, 0xe0, 0x00, 0xff },  // lime
    { 0x00, 0xe0, 0xe0, 0xff },  // cyan
    { 0xe0, 0x48, 0x48, 0xff },  // red
    { 0xe0, 0x48, 0xe0, 0xff },  // magenta
    { 0xe0, 0x64, 0x00, 0xff },  // dark orange
    { 0xee, 0x82, 0xee, 0xff },  // violet
    { 0xe0, 0xa0, 0x00, 0xff },  // orange
    { 0xb0, 0xb0, 0xff, 0xff },  // pink
    { 0xe0, 0xe0, 0x32, 0xff },  // yellow
    { 0xf0, 0xf0, 0xf0, 0xff },  // white
};

void vga_init()
{
    SDL_CreateWindowAndRenderer("fortuna-io-board emulator", 640 * 2, 480 * 2, 0, &window, &ren);
    vga_set_mode(V_640x480);
}

static void convert_framebuffer_to_32()
{
    for (size_t i = 0, j = 0; i < (fb->w * fb->h / 2); ++i) {
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

    if (texture)
        SDL_DestroyTexture(texture);
    fb_delete(fb);
    free(fb32);

    fb = fb_new(w, h);  // actual framebuffer
    fb32 = calloc(sizeof(SDL_Color), w * h);
    convert_framebuffer_to_32();

    texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, w, h);
    // SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
}

void vga_step()
{
    /*
    static char title[255];
    int fps = SDL_GetPerformanceFrequency();
    snprintf(title, sizeof title, "fortuna-io-board emulator - FPS %d", fps);
    SDL_SetWindowTitle(window, title);
    */

    SDL_SetRenderDrawColor(ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(ren, NULL);

    convert_framebuffer_to_32();
    SDL_UpdateTexture(texture, NULL, fb32, fb->w * 4);

    SDL_RenderTexture(ren, texture, NULL, NULL);

    SDL_RenderPresent(ren);
}

Framebuffer* vga_framebuffer()
{
    return fb;
}

int vga_width()
{
    return fb->w;
}

int vga_height()
{
    return fb->h;
}
