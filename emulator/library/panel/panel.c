#include "panel.h"

#include "config/config.h"

bool panel_led = false;
uint8_t panel_dipswitch = 0xff;

void panel_init()
{
    panel_dipswitch = config.panel;
}

void panel_init_interupts()
{
}

void panel_set_led(bool value)
{
    panel_led = value;
}

uint8_t panel_get_dipswitch()
{
    return panel_dipswitch;
}
