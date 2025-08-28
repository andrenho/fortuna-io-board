#include "fortuna.h"

#include <pico.h>

static void on_uart_rx()
{
    while (uart_is_readable(uart0)) {
        char ch = uart_getc(uart0);
        terminal_putc(ch);
    }
}

void terminal()
{
    terminal_start(fb_default_font());
    terminal_write("Fortuna I/O Board - VT200 terminal emulator via UART\r\n");

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
        Event e;
        while (fortuna_next_event(&e)) {
            if (e.type == E_KEYBOARD) {
                if (e.key.chr != 0 && e.key.pressed) {
                    // TODO - treat CTRL, SHIFT, special keys, etc
                    while (!uart_is_writable(uart0))
                        ;
                    uart_putc_raw(uart0, e.key.chr);
                }
            }
        }
    }

}