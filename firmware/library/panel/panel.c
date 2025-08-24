#include "panel.h"

#include <pico/types.h>

#include "fortuna.h"

#define LED_PIN 14
#define DIP0_PIN 27
#define DIP1_PIN 28
#define PUSHBUTTON_PIN 15

#define USER_BUTTON_DECOMPRESS 200

static absolute_time_t last_button_press[4] = { 0, 0, 0 };

void panel_init()
{
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_init(DIP0_PIN);
    gpio_init(DIP1_PIN);
    gpio_pull_up(DIP0_PIN);
    gpio_pull_up(DIP1_PIN);
    gpio_set_dir(DIP0_PIN, GPIO_IN);
    gpio_set_dir(DIP1_PIN, GPIO_IN);

    gpio_init(PUSHBUTTON_PIN);
    gpio_pull_up(PUSHBUTTON_PIN);
    gpio_set_dir(PUSHBUTTON_PIN, GPIO_IN);
}

static void irq_callback(uint gpio, uint32_t)
{
    PanelButton button = PB_UNKNOWN;
    switch (gpio) {
        case PUSHBUTTON_PIN: button = PB_PUSH_BUTTON; break;
        case DIP0_PIN: button = PB_SWITCH_0; break;
        case DIP1_PIN: button = PB_SWITCH_1; break;
    }

    if (to_ms_since_boot(last_button_press[button]) + USER_BUTTON_DECOMPRESS > to_ms_since_boot(get_absolute_time()))
        return;
    last_button_press[button] = get_absolute_time();

    Event e = {
        .type = E_PANEL,
        .panel = { .button = button, .new_value = !gpio_get(gpio) },
    };
    fortuna_add_event(&e);
}

void panel_init_interupts()
{
    gpio_set_irq_callback(irq_callback);

    gpio_set_irq_enabled(PUSHBUTTON_PIN, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(DIP0_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    gpio_set_irq_enabled(DIP1_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
    irq_set_enabled(IO_IRQ_BANK0, true);
}

void panel_set_led(bool value)
{
    gpio_put(LED_PIN, value);
}

uint8_t panel_get_dipswitch()
{
    uint8_t v = gpio_get(DIP1_PIN) << 1 | gpio_get(DIP0_PIN);
    return (~v) & 0b11;
}
