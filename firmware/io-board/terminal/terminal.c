#include "fortuna.h"

#include <pico.h>
#include <pico/util/queue.h>
#include <contrib/libtmt/tmt.h>

// https://gist.github.com/ConnerWill/d4b6c776b509add763e17f9f113fd25b
// https://claude.ai/public/artifacts/8b851796-06be-455c-8535-2e74ab6023db

#define MAX_QUEUE_SIZE (16 * 1024)
static queue_t ch_queue;

typedef struct {
    uint8_t     hid;
    const char* str;
} HidString;

static const HidString __in_flash() hid_strings[] = {
    { HID_KEY_ARROW_UP,    TMT_KEY_UP },
    { HID_KEY_ARROW_DOWN,  TMT_KEY_DOWN },
    { HID_KEY_ARROW_RIGHT, TMT_KEY_RIGHT },
    { HID_KEY_ARROW_LEFT,  TMT_KEY_LEFT },
    { HID_KEY_HOME,        TMT_KEY_HOME },
    { HID_KEY_END,         "\e[F" },
    { HID_KEY_INSERT,      TMT_KEY_INSERT },
    { HID_KEY_BACKSPACE,   TMT_KEY_BACKSPACE },
    { HID_KEY_TAB,         "\t" },
    { HID_KEY_PAGE_UP,     TMT_KEY_PAGE_UP },
    { HID_KEY_PAGE_DOWN,   TMT_KEY_PAGE_DOWN },
    { HID_KEY_F1,          TMT_KEY_F1 },
    { HID_KEY_F2,          TMT_KEY_F2 },
    { HID_KEY_F3,          TMT_KEY_F3 },
    { HID_KEY_F4,          TMT_KEY_F4 },
    { HID_KEY_F5,          TMT_KEY_F5 },
    { HID_KEY_F6,          TMT_KEY_F6 },
    { HID_KEY_F7,          TMT_KEY_F7 },
    { HID_KEY_F8,          TMT_KEY_F8 },
    { HID_KEY_F9,          TMT_KEY_F9 },
    { HID_KEY_F10,         TMT_KEY_F10 },
    { HID_KEY_F11,         "\e[23~" },
    { HID_KEY_F12,         "\e[24~" },
};

static void on_uart_rx()
{
    while (uart_is_readable(uart0)) {
        char ch = uart_getc(uart0);
        if (ch == 0)
            break;
        queue_try_add(&ch_queue, &ch);
    }
}

void terminal()
{
    terminal_start(fb_default_font());
    terminal_write("Fortuna I/O Board - VT200 terminal emulator via UART\r\n");

    queue_init(&ch_queue, sizeof(char), MAX_QUEUE_SIZE);

    gpio_set_function(0, UART_FUNCSEL_NUM(uart0, 0));
    gpio_set_function(1, UART_FUNCSEL_NUM(uart0, 1));

    // Initialise UART 0
    uart_init(uart0, 115200);
    uart_set_hw_flow(uart0, false, false);
    uart_set_fifo_enabled(uart0, false);

    // enable interrupts
    irq_set_exclusive_handler(UART0_IRQ, on_uart_rx);
    irq_set_enabled(UART0_IRQ, true);
    uart_set_irqs_enabled(uart0, true, false);

    for (;;) {
        char ch;
        if (queue_try_remove(&ch_queue, &ch))
            terminal_putc(ch);

        Event e;
skip:
        while (fortuna_next_event(&e)) {
            if (e.type == E_KEYBOARD && e.key.pressed) {

                while (!uart_is_writable(uart0))
                    ;

                if (e.key.hid_key == HID_KEY_ENTER || e.key.hid_key == HID_KEY_RETURN) {
                    uart_putc_raw(uart0, '\r');
                    goto skip;
                }

                if (!e.key.ctrl && !e.key.shift) {
                    for (size_t i = 0; i < sizeof hid_strings / sizeof hid_strings[0]; ++i) {
                        if (e.key.hid_key == hid_strings[i].hid) {
                            uart_puts(uart0, hid_strings[i].str);
                            goto skip;
                        }
                    }
                }
                if (e.key.chr != 0) {
                    if (e.key.ctrl) {
                        if (e.key.chr >= 'A' && e.key.chr <= '_')
                            uart_putc_raw(uart0, e.key.chr - 'A' + 1);
                        else if (e.key.chr >= 'a' && e.key.chr <= 'z')
                            uart_putc_raw(uart0, e.key.chr - 'a' + 1);
                    } else {
                        uart_putc_raw(uart0, e.key.chr);   // print actual char
                    }
                }
            }
        }
    }

}