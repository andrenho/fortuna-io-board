#include "framebuffer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "fortuna_font.h"

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

FColor fb_get_pixel_color(Framebuffer const* fb, uint16_t x, uint16_t y)
{
    const int pixel = ((fb->w * y) + x) ;
    if (pixel & 1)
        return (FColor) ((fb->data[pixel>>1] >> 4) & 0xf);
    else
        return (FColor) (fb->data[pixel>>1] & 0xf);
}

static void draw_hline(Framebuffer* fb, uint16_t x1, uint16_t y, uint16_t x2, FColor color)
{
    if (x1 & 1) {
        inline_draw_pixel(fb, x1, y, color);
        ++x1;
    }
    if (!(x2 & 1)) {
        inline_draw_pixel(fb, x2, y, color);
        --x2;
    }

    const int pixel = ((fb->w * y) + x1);
    memset(&fb->data[pixel >> 1], ((uint8_t) color << 4) | (uint8_t) color, (x2 >> 1) - (x1 >> 1) + 1);
}

static void draw_vline(Framebuffer* fb, uint16_t x, uint16_t y1, uint16_t y2, FColor color)
{
    for (uint16_t y = y1; y <= y2; ++y)
        inline_draw_pixel(fb, x, y, color);
}

static void swap(uint16_t a, uint16_t b) { uint16_t t = a; a = b; b = t; }

void fb_draw_line(Framebuffer* fb, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, FColor color)
{
    if (y1 == y2) {
        draw_hline(fb, x1, y1, x2, color);
    } else if (x1 == x2) {
        draw_vline(fb, x1, y1, y2, color);
    } else {
        int16_t steep = abs(y2 - y1) > abs(x2 - x1);
        if (steep) {
            swap(x1, y1);
            swap(x2, y2);
        }

        if (x1 > x2) {
            swap(x1, x2);
            swap(y1, y2);
        }

        int16_t dx, dy;
        dx = (int16_t) x2 - (int16_t) x1;
        dy = abs((int16_t) y2 - (int16_t) y1);

        int16_t err = dx / 2;
        int16_t ystep;

        if (y1 < y2) {
            ystep = 1;
        } else {
            ystep = -1;
        }

        for (; x1<=x2; x1++) {
            if (steep) {
                inline_draw_pixel(fb, y1, x1, color);
            } else {
                inline_draw_pixel(fb, x1, y1, color);
            }
            err -= dy;
            if (err < 0) {
                y1 += ystep;
                err += dx;
            }
        }
    }
}

void fb_draw_rectangle(Framebuffer* fb, uint16_t x, uint16_t y, uint16_t w, uint16_t h, FColor color)
{
    draw_hline(fb, x, y, x + w, color);
    draw_hline(fb, x, y + h, x + w, color);
    draw_vline(fb, x, y, y + h, color);
    draw_vline(fb, x + w, y, y + h, color);
}

void fb_draw_rectangle_filled(Framebuffer* fb, uint16_t x, uint16_t y, uint16_t w, uint16_t h, FColor color)
{
    for (int yy = y; yy < (y + h); ++yy)
        draw_hline(fb, x, yy, (x + w), color);
}

void fb_draw_ellipse(Framebuffer* fb, uint16_t xc, uint16_t yc, uint16_t rx, uint16_t ry, FColor color)
{
    float dx, dy, d1, d2, x, y;
    x = 0;
    y = ry;

    // Initial decision parameter of region 1
    d1 = (ry * ry)
         - (rx * rx * ry)
         + (0.25 * rx * rx);
    dx = 2 * ry * ry * x;
    dy = 2 * rx * rx * y;

    // For region 1
    while (dx < dy) {

        // Print points based on 4-way symmetry
        inline_draw_pixel(fb, x + xc, y + yc, color);
        inline_draw_pixel(fb, -x + xc, y + yc, color);
        inline_draw_pixel(fb, x + xc, -y + yc, color);
        inline_draw_pixel(fb, -x + xc, -y + yc, color);

        // Checking and updating value of
        // decision parameter based on algorithm
        if (d1 < 0) {
            x++;
            dx = dx + (2 * ry * ry);
            d1 = d1 + dx + (ry * ry);
        }
        else {
            x++;
            y--;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d1 = d1 + dx - dy + (ry * ry);
        }
    }

    // Decision parameter of region 2
    d2 = ((ry * ry) * ((x + 0.5) * (x + 0.5)))
         + ((rx * rx) * ((y - 1) * (y - 1)))
         - (rx * rx * ry * ry);

    // Plotting points of region 2
    while (y >= 0) {

        // printing points based on 4-way symmetry
        inline_draw_pixel(fb, x + xc, y + yc, color);
        inline_draw_pixel(fb, -x + xc, y + yc, color);
        inline_draw_pixel(fb, x + xc, -y + yc, color);
        inline_draw_pixel(fb, -x + xc, -y + yc, color);

        // Checking and updating parameter
        // value based on algorithm
        if (d2 > 0) {
            y--;
            dy = dy - (2 * rx * rx);
            d2 = d2 + (rx * rx) - dy;
        }
        else {
            y--;
            x++;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d2 = d2 + dx - dy + (rx * rx);
        }
    }
}

void fb_draw_ellipse_filled(Framebuffer* fb, uint16_t xc, uint16_t yc, uint16_t rx, uint16_t ry, FColor color)
{
    float dx, dy, d1, d2, x, y;
    x = 0;
    y = ry;

    // Initial decision parameter of region 1
    d1 = (ry * ry)
         - (rx * rx * ry)
         + (0.25 * rx * rx);
    dx = 2 * ry * ry * x;
    dy = 2 * rx * rx * y;

    // For region 1
    while (dx < dy) {

        // Print points based on 4-way symmetry
        draw_hline(fb, -x + xc, y + yc, x + xc, color);
        draw_hline(fb, -x + xc, -y + yc, x + xc, color);

        // Checking and updating value of
        // decision parameter based on algorithm
        if (d1 < 0) {
            x++;
            dx = dx + (2 * ry * ry);
            d1 = d1 + dx + (ry * ry);
        }
        else {
            x++;
            y--;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d1 = d1 + dx - dy + (ry * ry);
        }
    }

    // Decision parameter of region 2
    d2 = ((ry * ry) * ((x + 0.5) * (x + 0.5)))
         + ((rx * rx) * ((y - 1) * (y - 1)))
         - (rx * rx * ry * ry);

    // Plotting points of region 2
    while (y >= 0) {

        // printing points based on 4-way symmetry
        draw_hline(fb, -x + xc, y + yc, x + xc, color);
        draw_hline(fb, -x + xc, -y + yc, x + xc, color);

        // Checking and updating parameter
        // value based on algorithm
        if (d2 > 0) {
            y--;
            dy = dy - (2 * rx * rx);
            d2 = d2 + (rx * rx) - dy;
        }
        else {
            y--;
            x++;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d2 = d2 + dx - dy + (rx * rx);
        }
    }

}

void fb_fill_area(Framebuffer* fb, uint16_t x, uint16_t y, FColor color)
{
    typedef struct { uint16_t x, y; } Point;
    static Point stack[256];
    static int top = -1;

    // Get target color and check if fill needed
    FColor target = fb_get_pixel_color(fb, x, y);
    if (target == color) return;

    // Push starting point
    stack[++top] = (Point) {x, y};

    while (top >= 0) {
        uint16_t px = stack[top].x;
        uint16_t py = stack[top].y;
        top--;

        // Check bounds and color
        if (px >= fb->w || py >= fb->h) continue;
        if (fb_get_pixel_color(fb, px, py) != target) continue;

        // Find left and right bounds of this scanline
        uint16_t left = px, right = px;

        // Scan left
        while (left > 0 && fb_get_pixel_color(fb, left - 1, py) == target) {
            left--;
        }

        // Scan right
        while (right < fb->w - 1 && fb_get_pixel_color(fb, right + 1, py) == target) {
            right++;
        }

        // Fill the line
        draw_hline(fb, left, py, right, color);

        // Add points above and below to stack
        for (uint16_t i = left; i <= right; i++) {
            // Above
            if (py > 0 && top < 255 && fb_get_pixel_color(fb, i, py - 1) == target) {
                stack[++top] = (Point) {i, (uint16_t)(py - 1) };
            }
            // Below
            if (py < fb->h - 1 && top < 255 && fb_get_pixel_color(fb, i, py + 1) == target) {
                stack[++top] = (Point) {i, (uint16_t) (py + 1) };
            }
        }
    }
}

void fb_move_screen_up(Framebuffer* fb, uint16_t lines, FColor fill_color)
{
    uint32_t sz = lines * fb->w / 2;
    uint32_t vga_sz = fb->w * fb->h / 2;
    uint8_t color_data = ((uint8_t) fill_color << 4) | (uint8_t) fill_color;

    memmove(fb->data, &fb->data[sz], vga_sz - sz);
    memset(&fb->data[vga_sz - sz], color_data, sz);
}

static void draw_from_byte_bg(Framebuffer* fb, uint8_t byte, uint8_t n_bytes, uint16_t x, uint16_t y, FColor bg_color, FColor fg_color)
{
    for (int m = 0; m < n_bytes; ++m) {
        uint8_t v = byte & (1 << (n_bytes - m - 1));
        inline_draw_pixel(fb, x + m, y, v ? fg_color : bg_color);
    }
}

static void draw_from_byte(Framebuffer* fb, uint8_t byte, uint8_t n_bytes, uint16_t x, uint16_t y, FColor fg_color)
{
    for (int m = 0; m < n_bytes; ++m) {
        uint8_t v = byte & (1 << (n_bytes - m - 1));
        if (v)
            inline_draw_pixel(fb, x + m, y, fg_color);
    }
}

void fb_draw_character_bg(Framebuffer* fb, uint16_t px, uint16_t py, FFont const* font, uint8_t ch, FColor bg_color, FColor fg_color)
{
    uint8_t* pixel = &font->pixels[(ch - font->first_char) * font->char_height];
    for (uint8_t y = 0; y < font->char_height; ++y)
        draw_from_byte_bg(fb, *(pixel + y), font->char_width, px, py + y, bg_color, fg_color);
}

void fb_draw_character(Framebuffer* fb, uint16_t px, uint16_t py, FFont const* font, uint8_t ch, FColor fg_color)
{
    uint8_t* pixel = &font->pixels[(ch - font->first_char) * font->char_height];
    for (uint8_t y = 0; y < font->char_height; ++y)
        draw_from_byte(fb, *(pixel + y), font->char_width, px, py + y, fg_color);
}
