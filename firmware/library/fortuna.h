#ifndef FORTUNA_H
#define FORTUNA_H

#include <stdbool.h>
#include <stdint.h>

#include "vga/vga.h"
#include "vga/color.h"
#include "vga/framebuffer.h"
#include "vga/font.h"
#include "usb/usb.h"
#include "usb/keyboard.h"
#include "usb/mouse.h"
#include "usb/hid.h"
#include "terminal/terminal.h"
#include "terminal/spchars.h"
#include "panel/panel.h"
#include "rtc/rtc.h"

#include "hw_config.h"
#include "f_util.h"
#include "ff.h"

#define DEFAULT_QUEUE_SIZE 64

typedef enum { E_PANEL, E_KEYBOARD, E_MOUSE, E_EXTERNAL } EventType;

typedef struct __attribute__((packed)) Event {
    EventType type     : 8;
    union {
        KeyboardEvent key;
        MouseEvent    mouse;
        PanelEvent    panel;
    };
} Event;

void fortuna_init(uint16_t event_queue_size, void (*core1_step_function)(), int argc, char* argv[]);

bool fortuna_add_event(Event const* event);
bool fortuna_next_event(Event* event);

uint32_t fortuna_total_ram();
uint32_t fortuna_free_ram();

#endif //FORTUNA_H
