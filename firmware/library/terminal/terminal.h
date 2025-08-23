#ifndef TERMINAL_H_
#define TERMINAL_H_

#include "vga/font.h"

struct Event;

void terminal_start(FFont const* font);
void terminal_end();

void terminal_set_font(FFont const* font);

void terminal_putc(uint8_t c);
void terminal_write(const char* str);
void terminal_writef(const char* fmt, ...);

void terminal_putc_at(uint16_t row, uint16_t column, uint8_t c);
void terminal_write_at(uint16_t row, uint16_t column, const char* str);
void terminal_writef_at(uint16_t row, uint16_t column, const char* fmt, ...);

void terminal_set_cursor(uint8_t row, uint8_t column);
void terminal_draw_box(uint8_t row, uint8_t column, uint8_t width, uint8_t height, bool dbl);
void terminal_clear_screen();
void terminal_show_cursor(bool v);

uint8_t terminal_columns();
uint8_t terminal_rows();

bool terminal_active();
void terminal_resize();

uint16_t terminal_top_px();
uint16_t terminal_left_px();
uint16_t terminal_width_px();
uint16_t terminal_height_px();
FFont const* terminal_font();

#endif