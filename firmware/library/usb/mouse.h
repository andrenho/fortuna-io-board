#ifndef MOUSE_HH
#define MOUSE_HH

#include <stdint.h>

#include "tusb.h"

typedef enum {
    MB_LEFT   = 0b1,
    MB_RIGHT  = 0b10,
    MB_MIDDLE = 0b100,
} MouseButton;

typedef struct __attribute__((packed)) {
    int8_t  x, y;
    int8_t  wheel;
    uint8_t buttons;
} MouseEvent;

void mouse_init(uint8_t device_id_, uint8_t instance_);
void mouse_process_report(hid_mouse_report_t const *report);   // internal

#endif //MOUSE_HH
