#ifndef VGA_H
#define VGA_H

#include <stdbool.h>
#include <stdint.h>

typedef enum { V_640x480, V_640x240, V_320x240 } VgaMode;

void vga_init();
void vga_set_mode(VgaMode mode);
void vga_step();

int vga_width();
int vga_height();

void vga_show_pointer(bool v);
void vga_move_pointer(int8_t x, int8_t y);

struct Framebuffer* vga_framebuffer();

// void set_sprites(Sprite* sprites, uint16_t sz);
// void update_mouse_position(int8_t x, int8_t y);

#endif //VGA_H
