#include <stddef.h>
#include <stdio.h>

#include "fortuna.h"
#include "toshiba_font.h"
#include "ibm_font.h"
#include "vga_font.h"

static void command(uint8_t row, uint8_t column, const char* key, const char* command, bool selected)
{
    terminal_set_cursor(row, column);
    terminal_writef("[\e[30;42m %s \e[0m] %s", key, command);
    if (selected)
        terminal_write(" (*)");
}

static void draw()
{
    // main box
    terminal_draw_box(1, 1, terminal_columns(), terminal_rows() - 1, true);

    // separator line
    int y = vga_height() == 480 ? 4 : 3;
    terminal_set_cursor(y, 2);
    for (size_t i = 0; i < terminal_columns() - 2; ++i)
        terminal_putc(C_BOX_SINGLE_HORIZ);
    ++y;

    // info
    terminal_writef_at(y++, 3, "\e[1;33mFortuna I/O Board DEMO\e[0m (term %dx%d)", terminal_columns(), terminal_rows());

    // resolutions
    if (vga_height() == 480)
        ++y;
    uint16_t top_y = y;
    terminal_write_at(y++, 3, "Resolution:");
    command(y++, 4, "F1", "640x480", vga_framebuffer()->w == 640 && vga_framebuffer()->h == 480);
    command(y++, 4, "F2", "640x240", vga_framebuffer()->w == 640 && vga_framebuffer()->h == 240);
    command(y++, 4, "F3", "320x240", vga_framebuffer()->w == 320 && vga_framebuffer()->h == 240);

    // fonts
    y = top_y;
    uint16_t x = vga_width() == 640 ? 36 : 24;
    terminal_write_at(y++, x, "Fonts:");
    command(y++, x + 1, "F5", "Fortuna", terminal_font() == fb_default_font());
    command(y++, x + 1, "F6", "IBM", terminal_font() == &ibm_font);
    command(y++, x + 1, "F7", "Toshiba", terminal_font() == &toshiba_font);
    command(y++, x + 1, "F8", "VGA", terminal_font() == &vga_font);

    // draw colors
    {
        uint16_t x = terminal_left_px() + 10;
        uint16_t y = terminal_top_px() + 12;
        uint16_t w = terminal_width_px() / 16 - 5;
        uint16_t h = vga_height() / 16;
        for (uint8_t color = 0; color < 16; ++color) {
            fb_draw_rectangle_filled(vga_framebuffer(), x, y, w, h, color);
            if (color == C_BLACK)
                fb_draw_rectangle(vga_framebuffer(), x, y, w, h, C_BLUE);
            x += w + 4;
        }
    }

    // memory
    terminal_writef_at(terminal_rows() - 2, 3, "%d kB free out of %d kB", fortuna_free_ram() / 1024, fortuna_total_ram() / 1024);
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
                    if (e.key.pressed) {
                        printf("Key pressed -- HID 0x%02X, char 0x%02X (%c)\n", e.key.hid_key, e.key.chr, e.key.chr);
                        switch (e.key.hid_key) {
                            case HID_KEY_F1:
                                vga_set_mode(V_640x480);
                                draw();
                                break;
                            case HID_KEY_F2:
                                vga_set_mode(V_640x240);
                                draw();
                                break;
                            case HID_KEY_F3:
                                vga_set_mode(V_320x240);
                                draw();
                                break;
                            case HID_KEY_F5:
                                fb_clear(vga_framebuffer());
                                terminal_set_font(fb_default_font());
                                draw();
                                break;
                            case HID_KEY_F6:
                                terminal_set_font(&ibm_font);
                                draw();
                                break;
                            case HID_KEY_F7:
                                terminal_set_font(&toshiba_font);
                                draw();
                                break;
                            case HID_KEY_F8:
                                terminal_set_font(&vga_font);
                                draw();
                                break;
                        }
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