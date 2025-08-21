#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "tusb.h"

typedef struct __attribute__((packed)) KeyboardEvent {
    uint8_t hid_key;   // see tinyusb/src/class/hid/hid.h line 366
    uint8_t chr;
    bool    ctrl     : 1;
    bool    alt      : 1;
    bool    shift    : 1;
    bool    pressed  : 1;
} KeyboardEvent;

void keyboard_init(uint8_t device_id, uint8_t instance);             // internal
void keyboard_process_report(hid_keyboard_report_t const *report);   // internal

#endif //KEYBOARD_H
