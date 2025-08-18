#ifndef FORTUNA_H
#define FORTUNA_H

#include <stdint.h>

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

#endif //FORTUNA_H
