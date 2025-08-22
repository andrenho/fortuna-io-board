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

    terminal_start(fb_default_font());
    terminal_writef("Hello \e[0;36mworld %d\e[0m.", 42);
    terminal_draw_box(10, 10, 40, 4, false);

    // draw colors
    uint16_t sq_width = vga_width() / 17;
    uint16_t sq_height = vga_height() / 16;
    for (uint8_t i = 0; i < 16; ++i)
        fb_draw_rectangle_filled(vga_framebuffer(), sq_width * i + 10, 20, sq_width - 10, sq_height, i);

    for (;;) {
        Event e;
        while (fortuna_next_event(&e)) {
            // terminal_do_event(&e);
            switch (e.type) {
                case E_KEYBOARD:
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