#ifndef TERMINAL_H_
#define TERMINAL_H_

#include "vga/font.h"

struct Event;

void terminal_start(FFont const* font);
void terminal_end();

void terminal_write(const char* str);
void terminal_writef(const char* fmt, ...);

void terminal_set_cursor(uint8_t row, uint8_t column);
void terminal_draw_box(uint8_t row, uint8_t column, uint8_t width, uint8_t height, bool dbl);

#endif