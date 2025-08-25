#include <stddef.h>
#include <stdio.h>

#include "fortuna.h"
#include "toshiba_font.h"
#include "ibm_font.h"
#include "vga_font.h"

#ifdef FIRMWARE
#   include <pico/time.h>
#   define IN_FLASH __in_flash()
#else
#   include <SDL3/SDL.h>
#   define IN_FLASH
#endif

static int sdcard_line;
static size_t button_count = 0;

static const FMusic IN_FLASH music[] = {
    { C4, 200 },
    { D4, 200 },
    { E4, 200 },
    { F4, 200 },
    { PAUSE, 70 },
    { F4, 70 },
    { PAUSE, 70 },
    { F4, 150 },
    { PAUSE, 500 },
};

static void command(uint8_t row, uint8_t column, const char* key, const char* command, bool selected)
{
    terminal_set_cursor(row, column);
    terminal_writef("[\e[30;42m %s \e[0m] %s", key, command);
    if (selected)
        terminal_write(" (*)");
}

static void update_user_panel()
{
    terminal_writef_at(terminal_rows() - 3, 3, "Panel: DIP %d%d, button presses %zu",
        panel_get_dipswitch() >> 1, panel_get_dipswitch() & 1, button_count);
}

static void update_date()
{
    DateTime d = rtc_get();
    terminal_writef_at(terminal_rows() - 4, 3, "%02d/%02d/%04d %02d:%02d:%02d",
        d.month, d.day, d.year, d.hours, d.minutes, d.seconds);
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
    terminal_write_at(y++, 3, "\e[4;37mResolution:\e[0m");
    command(y++, 4, "F1", "640x480", vga_framebuffer()->w == 640 && vga_framebuffer()->h == 480);
    command(y++, 4, "F2", "640x240", vga_framebuffer()->w == 640 && vga_framebuffer()->h == 240);
    command(y++, 4, "F3", "320x240", vga_framebuffer()->w == 320 && vga_framebuffer()->h == 240);
    ++y;

    // other
    terminal_write_at(y++, 3, "\e[4;37mOther:\e[0m");
    sdcard_line = y;
    command(y++, 4, "S", "SDCard", false);
    command(y++, 4, "L", "Panel LED", false);
    command(y++, 4, "N", "Play single note", false);
    command(y++, 4, "P", "Play music", false);

    // fonts
    y = top_y;
    uint16_t x = vga_width() == 640 ? 36 : 24;
    terminal_write_at(y++, x, "\e[4;37mFonts:\e[0m");
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

    update_user_panel();
    update_date();
}

static void test_sdcard()
{
    FATFS fs;
    FRESULT fr = f_mount(&fs, "", 1);
    if (FR_OK != fr) {
        terminal_write_at(sdcard_line, 16, ": \e[0;31mERROR              \e[0m");
        printf("Error mounting SDCard: %s\n", FRESULT_str(fr));
        f_unmount("");
        return;
    }

    DIR dir;
    fr = f_opendir(&dir, "/");
    if (fr != FR_OK) {
        terminal_write_at(sdcard_line, 16, ": \e[0;31mERROR              \e[0m");
        printf("Error reading directory: %s\n", FRESULT_str(fr));
        f_unmount("");
        return;
    }

    size_t file_count = 0;
    for (;;) {
        static FILINFO fno;
        fr = f_readdir(&dir, &fno);
        if (fno.fname[0] == 0)
            break;
        ++file_count;
    }
    f_closedir(&dir);

    terminal_writef_at(sdcard_line, 16, ": \e[1;32m%d file(s) in root\e[0m", file_count);
}

#ifdef FIRMWARE

static bool timer_callback(repeating_timer_t* rt)
{
    (void) rt;
    update_date();
    return true;
}

static void add_clock_timer()
{
    static repeating_timer_t timer;
    add_repeating_timer_ms(1000, timer_callback, NULL, &timer);
}

#else

static Uint32 timer_callback(void *userdata, SDL_TimerID timerID, Uint32 interval)
{
    update_date();
    return interval;
}

static void add_clock_timer()
{
    SDL_AddTimer(1000, timer_callback, NULL);
}

#endif

int main(int argc, char* argv[])
{
    fortuna_init(DEFAULT_QUEUE_SIZE, NULL, argc, argv);

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

    add_clock_timer();

    audio_set_music(music, sizeof(music) / sizeof music[0]);

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
                                vga_set_mode(V_640x480); draw();
                                break;
                            case HID_KEY_F2:
                                vga_set_mode(V_640x240); draw();
                                break;
                            case HID_KEY_F3:
                                vga_set_mode(V_320x240); draw();
                                break;
                            case HID_KEY_F5:
                                fb_clear(vga_framebuffer());
                                terminal_set_font(fb_default_font());
                                draw();
                                break;
                            case HID_KEY_F6:
                                terminal_set_font(&ibm_font); draw();
                                break;
                            case HID_KEY_F7:
                                terminal_set_font(&toshiba_font); draw();
                                break;
                            case HID_KEY_F8:
                                terminal_set_font(&vga_font); draw();
                                break;
                        }
                        switch (e.key.chr) {
                            case 's': case 'S':
                                test_sdcard();
                                break;
                            case 'l': case 'L': {
                                static bool led = false;
                                led = !led;
                                panel_set_led(led);
                                break;
                            }
                            case 'p': case 'P':
                                audio_play_music(false);
                                break;
                            case 'n': case 'N':
                                audio_play_single_note(&(FMusic) { C4, 500 });
                                break;
                        }
                    }
                    break;
                case E_PANEL:
                    if (e.panel.button == PB_PUSH_BUTTON)
                        ++button_count;
                    update_user_panel();
                    break;
                case E_MOUSE:
                    break;
                case E_EXTERNAL:
                    break;
            }
        }
    }
}