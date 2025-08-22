#include "terminal.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "contrib/libtmt/tmt.h"

#include "vga/vga.h"
#include "vga/framebuffer.h"

#define MAX(a, b) ((a)>(b)?(a):(b))

static TMT*         vt = NULL;
static uint16_t     columns, lines;
static uint16_t     rx = 0, ry = 0;
static FFont const* font;
static TMTPOINT     prev_cursor = { 0, 0 };
static bool         blink_on = true;

#define BLINK_TIMER_MS 500

#ifdef FIRMWARE
#   include <pico/time.h>
#   define IN_FLASH __in_flash()
#else
#   include <SDL3/SDL.h>
#   define IN_FLASH
#endif

/*
static const char* IN_FLASH alt_charset = {
    0x1a, 0x1b, 0x18, 0x19, 0xfe, 0x04, 0xb1, 0xf8,
};
*/

static FColor translate_color(tmt_color_t color, bool bold, bool is_bg)
{
    switch (color) {
        case TMT_COLOR_BLACK:
            return bold ? C_DARK_GREEN : C_BLACK;
        case TMT_COLOR_RED:
            return bold ? C_DARK_ORANGE : C_RED;
        case TMT_COLOR_GREEN:
            return bold ? C_LIME : C_GREEN;
        case TMT_COLOR_YELLOW:
            return bold ? C_YELLOW : C_ORANGE;
        case TMT_COLOR_BLUE:
            return bold ? C_SKY_BLUE : C_BLUE;
        case TMT_COLOR_MAGENTA:
            return bold ? C_VIOLET : C_MAGENTA;
        case TMT_COLOR_CYAN:
            return bold ? C_CYAN : C_SKY_BLUE;
        case TMT_COLOR_WHITE:
            return C_WHITE;
        default:
            return is_bg ? C_BLACK : C_WHITE;
    }
}

static void draw_char(uint16_t row, uint16_t column, TMTCHAR c, TMTPOINT const* cu)
{
    uint16_t x = column * font->char_width + rx;
    uint16_t y = row * font->char_height + ry;

    FColor bg_color = translate_color(c.a.bg, false, true);
    FColor fg_color = translate_color(c.a.fg, c.a.bold, false);

    if (cu->r == row && cu->c == column && blink_on) {
        fg_color = bg_color;
        bg_color = C_LIME;
    } else if (c.a.reverse) {
        FColor tmp = fg_color;
        fg_color = bg_color;
        bg_color = tmp;
    }

    if (c.a.invisible)
        fg_color = bg_color;

    fb_draw_character_bg(vga_framebuffer(), x, y, font, c.c, bg_color, fg_color);
}

static void callback(tmt_msg_t m, TMT *vt, const void *a, void *p)
{
    const TMTSCREEN *s = tmt_screen(vt);
    const TMTPOINT *cu = tmt_cursor(vt);

    switch (m) {
        case TMT_MSG_UPDATE:
            for (size_t r = 0; r < s->nline; r++)
                if (s->lines[r]->dirty)
                    for (size_t c = 0; c < s->ncol; c++)
                        draw_char(r, c, s->lines[r]->chars[c], cu);
            tmt_clean(vt);
            break;
        case TMT_MSG_BELL:
            break;
        case TMT_MSG_MOVED:
            draw_char(prev_cursor.r, prev_cursor.c, s->lines[prev_cursor.r]->chars[prev_cursor.c], cu);
            prev_cursor = *cu;
            draw_char(cu->r, cu->c, s->lines[cu->r]->chars[cu->c], cu);
            break;
        case TMT_MSG_ANSWER:
            break;
        case TMT_MSG_CURSOR:
            break;
    }
}

static bool redraw_char_on_cursor(TMT* vt)
{
    blink_on = !blink_on;
    const TMTPOINT *cu = tmt_cursor(vt);
    const TMTSCREEN *s = tmt_screen(vt);
    draw_char(cu->r, cu->c, s->lines[cu->r]->chars[cu->c], cu);
    return vt != NULL;
}

#ifdef FIRMWARE

static bool timer_callback(repeating_timer_t* rt)
{
    return redraw_char_on_cursor(* (TMT**) rt->user_data);
}

static void add_blink_timer()
{
    static repeating_timer_t timer;
    add_repeating_timer_ms(BLINK_TIMER_MS, timer_callback, &vt, &timer);
}

#else

static Uint32 timer_callback(void *userdata, SDL_TimerID timerID, Uint32 interval)
{
    return redraw_char_on_cursor(* (TMT**) userdata) ? BLINK_TIMER_MS : 0;
}

static void add_blink_timer()
{
    SDL_AddTimer(BLINK_TIMER_MS, timer_callback, &vt);
}

#endif

void terminal_start(FFont const* font_)
{
    font = font_;

    columns = MAX(80, (vga_width() / font->char_width));
    lines = MAX(25, (vga_height() / font->char_height));

    rx = (vga_width() / 2) - (columns * font->char_width / 2);
    ry = (vga_height() / 2) - (lines * font->char_height / 2);

    vt = tmt_open(lines, columns, callback, NULL, NULL);

    add_blink_timer();
}

void terminal_write(const char* str)
{
    tmt_write(vt, str, 0);
}

void terminal_writef(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int sz = vsnprintf(NULL, 0, fmt, args);
    char* buf = malloc(sz + 1);
    vsnprintf(buf, sz + 1, fmt, args);
    terminal_write(buf);
    free(buf);
    va_end(args);
}

void terminal_end()
{
    tmt_close(vt);
    vt = NULL;
}

void terminal_set_cursor(uint8_t row, uint8_t column)
{
    terminal_writef("\e[%d;%dH", row, column);
}

void terminal_draw_box(uint8_t row, uint8_t column, uint8_t width, uint8_t height, bool dbl)
{

}
