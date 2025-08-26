#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

#include "color.h"
#include "font.h"

typedef struct Framebuffer {
    uint16_t w;
    uint16_t h;
    uint8_t* data;
} Framebuffer;

Framebuffer* fb_new(uint16_t w, uint16_t h);
void         fb_delete(Framebuffer* fb);

void         fb_clear(Framebuffer* fb);

void         fb_set_pixel(Framebuffer* fb, int16_t x, int16_t y, FColor color);
FColor       fb_get_pixel_color(Framebuffer const* fb, uint16_t x, uint16_t y);

void         fb_draw_line(Framebuffer* fb, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, FColor color);
void         fb_draw_rectangle(Framebuffer* fb, uint16_t x, uint16_t y, uint16_t w, uint16_t h, FColor color);
void         fb_draw_rectangle_filled(Framebuffer* fb, uint16_t x, uint16_t y, uint16_t w, uint16_t h, FColor color);
void         fb_draw_ellipse(Framebuffer* fb, uint16_t x, uint16_t y, uint16_t w, uint16_t h, FColor color);
void         fb_draw_ellipse_filled(Framebuffer* fb, uint16_t x, uint16_t y, uint16_t w, uint16_t h, FColor color);

void         fb_fill_area(Framebuffer* fb, uint16_t x, uint16_t y, FColor color);
void         fb_move_screen_up(Framebuffer* fb, uint16_t lines, FColor fill_color);

void         fb_draw_character_bg(Framebuffer* fb, uint16_t x, uint16_t y, FFont const* font, uint8_t ch, FColor bg_color, FColor fg_color);
void         fb_draw_character(Framebuffer* fb, uint16_t x, uint16_t y, FFont const* font, uint8_t ch, FColor fg_color);

FFont const* fb_default_font();

void         fb_resize(Framebuffer* fb, uint16_t w, uint16_t h);

bool         fb_draw_image(Framebuffer* fb, uint8_t const* image, uint16_t x, uint16_t y);


// void fb_setup(ScreenMode mode);

static __attribute__((always_inline)) inline uint32_t fb_pixel_idx(Framebuffer* fb, uint16_t x, uint16_t y)
{
    if (fb->w == 640) {
        if (fb->h == 480)
            return (640 >> 1) * y + (x >> 1);
        if (fb->h == 240)
            return (640 >> 1) * (y >> 1) + (x >> 1);
    } else if (fb->w == 320) {
        return (320 >> 1) * (y >> 1) + (x >> 1);
    }

    return 0;
}

#endif //FRAMEBUFFER_H
