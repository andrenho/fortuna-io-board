#ifndef FORTUNA_H
#define FORTUNA_H

#include <stdbool.h>
#include <stdint.h>

#include "vga.h"
#include "color.h"
#include "framebuffer.h"

#define DEFAULT_QUEUE_SIZE 64

typedef enum { UserPanel, Keyboard, Mouse, External } EventType;

typedef struct __attribute__((packed)) {
    EventType type     : 8;
    union {
        /*
        user::Event          user;
        usb::keyboard::Event key;
        usb::mouse::Event    mouse;
        external::Buffer     external;
        */
    };
} Event;

void fortuna_init(uint16_t event_queue_size, void (*core1_step_function)());

void fortuna_add_event(Event const* event);
bool fortuna_next_event(Event* event);

#endif //FORTUNA_H
