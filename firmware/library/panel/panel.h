#ifndef PANEL_H
#define PANEL_H

#include <stdint.h>

void    panel_init();
void    panel_init_interupts();

void    panel_set_led(bool value);
uint8_t panel_get_dipswitch();

typedef enum { PB_PUSH_BUTTON, PB_SWITCH_0, PB_SWITCH_1, PB_UNKNOWN } PanelButton;

typedef struct __attribute__((packed)) PanelEvent {
    PanelButton button;
    bool        new_value;
} PanelEvent;

#endif //PANEL_H
