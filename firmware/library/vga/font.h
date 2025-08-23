#ifndef FONT_H_
#define FONT_H_

#include <stdint.h>

typedef struct {
    uint8_t        char_width;
    uint8_t        char_height;
    uint8_t        first_char;
    uint8_t        last_char;
    uint8_t const* pixels;
} FFont;

#endif