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

#define DEFAULT_QUEUE_SIZE 64

typedef enum { E_USER_PANEL, E_KEYBOARD, E_MOUSE, E_EXTERNAL } EventType;

typedef struct __attribute__((packed)) {
    EventType type     : 8;
    union {
        KeyboardEvent key;
        /*
        user::Event          user;
        usb::keyboard::Event key;
        usb::mouse::Event    mouse;
        external::Buffer     external;
        */
    };
} Event;

void fortuna_init(uint16_t event_queue_size, void (*core1_step_function)());

bool fortuna_add_event(Event const* event);
bool fortuna_next_event(Event* event);

#endif //FORTUNA_H
