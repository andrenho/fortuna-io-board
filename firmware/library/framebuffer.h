#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

#include "color.h"

typedef struct Framebuffer {
    uint16_t w;
    uint16_t h;
    uint8_t* data;
} Framebuffer;

Framebuffer* fb_new(uint16_t w, uint16_t h);
void         fb_delete(Framebuffer* fb);

void         fb_set_pixel(Framebuffer* fb, int16_t x, int16_t y, FColor color);

// void fb_setup(ScreenMode mode);

#endif //FRAMEBUFFER_H
