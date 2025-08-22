#include <stddef.h>
#include <stdio.h>

#include "fortuna.h"

static void command(uint8_t row, uint8_t column, const char* key, const char* command, bool selected)
{
    terminal_set_cursor(row, column);
    terminal_writef("[\e[30;42m %s \e[0m] %s", key, command);
    if (selected)
        terminal_write(" (selected)");
}

static void draw()
{
    // main box
    terminal_draw_box(1, 1, terminal_columns(), terminal_rows() - 1, true);

    // separator line
    terminal_set_cursor(5, 2);
    for (size_t i = 0; i < terminal_columns() - 2; ++i)
        terminal_putc(C_BOX_SINGLE_HORIZ);

    // info
    terminal_set_cursor(6, 3);
    terminal_write("Fortuna I/O Board DEMO");

    // resolutions
    command( 8, 3, "F1", "Resolution: 640x480", vga_framebuffer()->w == 640 && vga_framebuffer()->h == 480);
    command( 9, 3, "F2", "Resolution: 640x240", vga_framebuffer()->w == 640 && vga_framebuffer()->h == 240);
    command(10, 3, "F3", "Resolution: 320x240", vga_framebuffer()->w == 320 && vga_framebuffer()->h == 240);

    // draw colors
    uint16_t sq_width = vga_width() / 17;
    uint16_t sq_height = vga_height() / 16;
    for (uint8_t i = 0; i < 16; ++i)
        fb_draw_rectangle_filled(vga_framebuffer(), sq_width * i + 10, 20, sq_width - 10, sq_height, i);
}

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
    terminal_show_cursor(false);

    draw();

    for (;;) {
        Event e;
        while (fortuna_next_event(&e)) {
            // terminal_do_event(&e);
            switch (e.type) {
                case E_KEYBOARD:
                    if (e.key.pressed)
                        printf("Key pressed -- HID 0x%02X, char 0x%02X (%c)\n", e.key.hid_key, e.key.chr, e.key.chr);
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