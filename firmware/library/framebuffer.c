#include "framebuffer.h"

#include <stdlib.h>
#include <stdio.h>

#define TOPMASK    0b00001111
#define BOTTOMMASK 0b11110000

Framebuffer* fb_new(uint16_t w, uint16_t h)
{
    Framebuffer* fb = calloc(1, sizeof(Framebuffer));
    fb->w = w;
    fb->h = h;
    fb->data = calloc(1, w * h / 2);
    return fb;
}

void fb_delete(Framebuffer* fb)
{
    free(fb);
}

static void __attribute__((always_inline)) inline inline_draw_pixel(Framebuffer* fb, uint16_t x, uint16_t y, FColor color)
{
    const uint32_t fb_idx = 0; // (fb->w * fb->h) >> 1;0

    // Which pixel is it?
    const int pixel = ((fb->w * y) + x) ;

    printf("%d\n", pixel);

    // Is this pixel stored in the first 4 bits
    // of the vga data array index, or the second
    // 4 bits? Check, then mask.
    if (pixel & 1)
        fb->data[fb_idx + (pixel>>1)] = (fb->data[fb_idx + (pixel>>1)] & TOPMASK) | ((uint8_t) color << 4) ;
    else
        fb->data[fb_idx + (pixel>>1)] = (fb->data[fb_idx + (pixel>>1)] & BOTTOMMASK) | ((uint8_t) color) ;
}

void fb_set_pixel(Framebuffer* fb, int16_t x, int16_t y, FColor color)
{
    if (x < 0 || y < 0 || x >= fb->w || y >= fb->h)
        return;

    inline_draw_pixel(fb, x, y, color);
}