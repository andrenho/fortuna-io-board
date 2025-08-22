#ifndef TERMINAL_H_
#define TERMINAL_H_

#include "vga/font.h"

struct Event;

void terminal_start(FFont const* font);
void terminal_end();

void terminal_write(const char* str);

#endif