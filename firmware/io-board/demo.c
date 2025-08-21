#include <stddef.h>

#include "fortuna.h"

int main()
{
    fortuna_init(DEFAULT_QUEUE_SIZE, NULL);

    // fixating points (used to calibrate the monitor)
    fb_set_pixel(vga_framebuffer(), 0, 0, C_NAVY_BLUE);
    fb_set_pixel(vga_framebuffer(), 0, vga_height() - 1, C_NAVY_BLUE);
    fb_set_pixel(vga_framebuffer(), vga_width() - 1, 0, C_NAVY_BLUE);
    fb_set_pixel(vga_framebuffer(), vga_width() - 1, vga_height() - 1, C_NAVY_BLUE);

    // draw colors
    uint16_t sq_width = vga_width() / 16 - 16;
    uint16_t sq_height = vga_height() / 16;
    for (uint8_t i = 0; i < 16; ++i)
        fb_draw_rectangle_filled(vga_framebuffer(), 10 + ((sq_width + 10) * i), 20, sq_width, sq_height, i);

    for (;;) {
        Event e;
        while (fortuna_next_event(&e)) {

        }
    }

}