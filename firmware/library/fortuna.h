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

#define DEFAULT_QUEUE_SIZE 64

typedef enum { E_USER_PANEL, E_KEYBOARD, E_MOUSE, E_EXTERNAL } EventType;

typedef struct __attribute__((packed)) Event {
    EventType type     : 8;
    union {
        KeyboardEvent key;
        MouseEvent    mouse;
        /*
        user::Event          user;
        external::Buffer     external;
        */
    };
} Event;

void fortuna_init(uint16_t event_queue_size, void (*core1_step_function)());

bool fortuna_add_event(Event const* event);
bool fortuna_next_event(Event* event);

#endif //FORTUNA_H
