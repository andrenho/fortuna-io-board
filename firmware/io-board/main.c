#include "fortuna.h"

extern void demo();
extern void terminal();

int main(int argc, char* argv[])
{
    fortuna_init(DEFAULT_QUEUE_SIZE, NULL, argc, argv);

    // fixating points (used to calibrate the monitor)
    fb_set_pixel(vga_framebuffer(), 0, 0, C_NAVY_BLUE);
    fb_set_pixel(vga_framebuffer(), 0, vga_height() - 1, C_NAVY_BLUE);
    fb_set_pixel(vga_framebuffer(), vga_width() - 1, 0, C_NAVY_BLUE);
    fb_set_pixel(vga_framebuffer(), vga_width() - 1, vga_height() - 1, C_NAVY_BLUE);

    switch (panel_get_dipswitch()) {
        case 0b00:
            demo();
        case 0b01:
            terminal();
        default:
            terminal_start(fb_default_font());
            terminal_write("DIP switch configuration not supported.");
            for (;;);
    }
}