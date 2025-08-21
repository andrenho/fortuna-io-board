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

// void         fb_draw_image(Image const& image, uint16_t x, uint16_t y, uint8_t framebuffer=0);


// void fb_setup(ScreenMode mode);

#endif //FRAMEBUFFER_H
