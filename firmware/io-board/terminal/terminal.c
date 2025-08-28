#include "fortuna.h"

#include <pico.h>
#include <pico/util/queue.h>
#include <contrib/libtmt/tmt.h>

#define MAX_QUEUE_SIZE (16 * 1024)
static queue_t ch_queue;

static void on_uart_rx()
{
    while (uart_is_readable(uart0)) {
        char ch = uart_getc(uart0);
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
    uart_set_irq_enables(uart0, true, false);

    for (;;) {
        char ch;
        if (queue_try_remove(&ch_queue, &ch))
            terminal_putc(ch);

        Event e;
        while (fortuna_next_event(&e)) {
            if (e.type == E_KEYBOARD && e.key.pressed) {
                while (!uart_is_writable(uart0))
                    ;
                switch (e.key.hid_key) {
                    case HID_KEY_ARROW_UP:
                        uart_puts(uart0, TMT_KEY_UP);
                        break;
                    case HID_KEY_ARROW_DOWN:
                        uart_puts(uart0, TMT_KEY_DOWN);
                        break;
                    case HID_KEY_ARROW_RIGHT:
                        uart_puts(uart0, TMT_KEY_RIGHT);
                        break;
                    case HID_KEY_ARROW_LEFT:
                        uart_puts(uart0, TMT_KEY_LEFT);
                        break;
                    case HID_KEY_HOME:
                        uart_puts(uart0, TMT_KEY_HOME);
                        break;
                    case HID_KEY_END:
                        uart_puts(uart0, TMT_KEY_END);
                        break;
                    case HID_KEY_INSERT:
                        uart_puts(uart0, TMT_KEY_INSERT);
                        break;
                    case HID_KEY_BACKSPACE:
                        uart_puts(uart0, TMT_KEY_BACKSPACE);
                        break;
                    case HID_KEY_TAB:
                        uart_puts(uart0, "\t");
                        break;
                    case HID_KEY_PAGE_UP:
                        uart_puts(uart0, TMT_KEY_PAGE_UP);
                        break;
                    case HID_KEY_PAGE_DOWN:
                        uart_puts(uart0, TMT_KEY_PAGE_UP);
                        break;
                    case HID_KEY_F1:
                        uart_puts(uart0, TMT_KEY_F1);
                        break;
                    case HID_KEY_F2:
                        uart_puts(uart0, TMT_KEY_F2);
                        break;
                    case HID_KEY_F3:
                        uart_puts(uart0, TMT_KEY_F3);
                        break;
                    case HID_KEY_F4:
                        uart_puts(uart0, TMT_KEY_F4);
                        break;
                    case HID_KEY_F5:
                        uart_puts(uart0, TMT_KEY_F5);
                        break;
                    case HID_KEY_F6:
                        uart_puts(uart0, TMT_KEY_F6);
                        break;
                    case HID_KEY_F7:
                        uart_puts(uart0, TMT_KEY_F7);
                        break;
                    case HID_KEY_F8:
                        uart_puts(uart0, TMT_KEY_F8);
                        break;
                    case HID_KEY_F9:
                        uart_puts(uart0, TMT_KEY_F9);
                        break;
                    case HID_KEY_F10:
                        uart_puts(uart0, TMT_KEY_F10);
                        break;
                    case HID_KEY_ENTER:
                    case HID_KEY_RETURN:
                        uart_putc(uart0, '\r');
                        break;
                    default:
                        if (e.key.chr != 0) {
                            // TODO - treat CTRL, SHIFT, special keys, etc
                            uart_putc_raw(uart0, e.key.chr);
                        }
                        break;
                }
            }
        }
    }

}