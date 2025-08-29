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
    { HID_KEY_ESCAPE,      "\e" },
};

static const HidString __in_flash() hid_strings_ctrl[] = {
    { HID_KEY_ARROW_UP,    "\e[1;5A" },
    { HID_KEY_ARROW_DOWN,  "\e[1;5B" },
    { HID_KEY_ARROW_RIGHT, "\e[1;5C" },
    { HID_KEY_ARROW_LEFT,  "\e[1;5D" },
    { HID_KEY_HOME,        "\e[1;5H" },
    { HID_KEY_END,         "\e[1;5F" },
    { HID_KEY_INSERT,      "\e[2;5~" },
    { HID_KEY_BACKSPACE,   "\b" },
    { HID_KEY_TAB,         "\t" },
    { HID_KEY_PAGE_UP,     "\e[5;5~" },
    { HID_KEY_PAGE_DOWN,   "\e[6;5~" },
    { HID_KEY_F1,          "\e[1;5P" },
    { HID_KEY_F2,          "\e[1;5Q" },
    { HID_KEY_F3,          "\e[1;5R" },
    { HID_KEY_F4,          "\e[1;5S" },
    { HID_KEY_F5,          "\e[15;5~" },
    { HID_KEY_F6,          "\e[17;5~" },
    { HID_KEY_F7,          "\e[18;5~" },
    { HID_KEY_F8,          "\e[19;5~" },
    { HID_KEY_F9,          "\e[20;5~" },
    { HID_KEY_F10,         "\e[21;5~" },
    { HID_KEY_F11,         "\e[23;5~" },
    { HID_KEY_F12,         "\e[24;5~" },
    { HID_KEY_ESCAPE,      "\e" },
};

static const HidString __in_flash() hid_strings_shift[] = {
    { HID_KEY_ARROW_UP,    "\e[1;2A" },
    { HID_KEY_ARROW_DOWN,  "\e[1;2B" },
    { HID_KEY_ARROW_RIGHT, "\e[1;2C" },
    { HID_KEY_ARROW_LEFT,  "\e[1;2D" },
    { HID_KEY_HOME,        "\e[1;2H" },
    { HID_KEY_END,         "\e[1;2F" },
    { HID_KEY_INSERT,      "\e[2;5~" },  // TODO
    { HID_KEY_BACKSPACE,   "\x7f" },
    { HID_KEY_TAB,         "\e[Z" },
    { HID_KEY_PAGE_UP,     TMT_KEY_PAGE_UP },  // TODO
    { HID_KEY_PAGE_DOWN,   TMT_KEY_PAGE_DOWN },
    { HID_KEY_F1,          "\e[1;2P" },
    { HID_KEY_F2,          "\e[1;2Q" },
    { HID_KEY_F3,          "\e[1;2R" },
    { HID_KEY_F4,          "\e[1;2S" },
    { HID_KEY_F5,          "\e[15;2~" },
    { HID_KEY_F6,          "\e[17;2~" },
    { HID_KEY_F7,          "\e[18;2~" },
    { HID_KEY_F8,          "\e[19;2~" },
    { HID_KEY_F9,          "\e[20;2~" },
    { HID_KEY_F10,         "\e[21;2~" },
    { HID_KEY_F11,         "\e[23;2~" },
    { HID_KEY_F12,         "\e[24;2~" },
    { HID_KEY_ESCAPE,      "\e" },
};


static const HidString __in_flash() hid_strings_ctrl_shift[] = {
    { HID_KEY_ARROW_UP,    "\e[1;6A" },
    { HID_KEY_ARROW_DOWN,  "\e[1;6B" },
    { HID_KEY_ARROW_RIGHT, "\e[1;6C" },
    { HID_KEY_ARROW_LEFT,  "\e[1;6D" },
    { HID_KEY_HOME,        "\e[1;6H" },
    { HID_KEY_END,         "\e[1;6F" },
    { HID_KEY_INSERT,      "\e[2;6~" },
    { HID_KEY_BACKSPACE,   "\b" },
    { HID_KEY_TAB,         "\t" },
    { HID_KEY_PAGE_UP,     "\e[5;6~" },
    { HID_KEY_PAGE_DOWN,   "\e[6;6~" },
    { HID_KEY_F1,          "\e[1;6P" },
    { HID_KEY_F2,          "\e[1;6Q" },
    { HID_KEY_F3,          "\e[1;6R" },
    { HID_KEY_F4,          "\e[1;6S" },
    { HID_KEY_F5,          "\e[15;6~" },
    { HID_KEY_F6,          "\e[17;6~" },
    { HID_KEY_F7,          "\e[18;6~" },
    { HID_KEY_F8,          "\e[19;6~" },
    { HID_KEY_F9,          "\e[20;6~" },
    { HID_KEY_F10,         "\e[21;6~" },
    { HID_KEY_F11,         "\e[23;6~" },
    { HID_KEY_F12,         "\e[24;6~" },
    { HID_KEY_ESCAPE,      "\e" },
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