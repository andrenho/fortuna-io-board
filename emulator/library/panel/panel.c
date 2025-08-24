#include "panel.h"

#include "config/config.h"

bool panel_led = false;
bool panel_dipswitch[2] = { 0, 0 };

void panel_init()
{
    panel_dipswitch[0] = config.panel & 1;
    panel_dipswitch[1] = config.panel >> 1;
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
    return panel_dipswitch[0] | (panel_dipswitch[1] << 1);
}
