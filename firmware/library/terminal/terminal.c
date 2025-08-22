#include "terminal.h"

#include <stdio.h>

#include "contrib/libtmt/tmt.h"

#include "vga/vga.h"
#include "vga/framebuffer.h"

static TMT*         vt = NULL;
static uint16_t     columns, lines;
static uint16_t     rx = 0, ry = 0;
static FFont const* font;

static void draw_char(uint16_t row, uint16_t column, TMTCHAR c)
{
    uint16_t x = column * font->char_width + rx;
    uint16_t y = row * font->char_height + ry;
    FColor bg_color = C_BLACK;  // TODO
    FColor fg_color = C_WHITE;  // TODO

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
                        draw_char(r, c, s->lines[r]->chars[c]);
            tmt_clean(vt);
            break;
        case TMT_MSG_BELL:
            break;
        case TMT_MSG_MOVED:
            break;
        case TMT_MSG_ANSWER:
            break;
        case TMT_MSG_CURSOR:
            break;
    }
}

void terminal_start(FFont const* font_)
{
    font = font_;

    columns = vga_width() / font->char_width;
    lines = vga_height() / font->char_height;

    vt = tmt_open(lines, columns, callback, NULL, NULL);
    printf("%d\n", tmt_screen(vt)->nline);
}

void terminal_write(const char* str)
{
    tmt_write(vt, str, 0);
}

void terminal_end()
{
    tmt_close(vt);
}

void terminal_do_event(struct Event* event)
{
}