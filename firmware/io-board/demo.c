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

    // mouse
    vga_show_pointer(true);

    // draw colors
    uint16_t sq_width = vga_width() / 17;
    uint16_t sq_height = vga_height() / 16;
    for (uint8_t i = 0; i < 16; ++i)
        fb_draw_rectangle_filled(vga_framebuffer(), sq_width * i + 10, 20, sq_width - 10, sq_height, i);

    int cw = 0;

    for (;;) {
        Event e;
        while (fortuna_next_event(&e)) {
            switch (e.type) {
                case E_KEYBOARD:
                    if (e.key.pressed) {
                        fb_draw_character(vga_framebuffer(), cw, 60, DEFAULT_FONT, e.key.chr, C_WHITE);
                        cw += 7;
                    }
                    break;
                case E_USER_PANEL:
                    break;
                case E_MOUSE:
                    break;
                case E_EXTERNAL:
                    break;
            }
        }
    }
}