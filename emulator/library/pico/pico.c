#include "pico.h"

void gpio_set_function(unsigned int gpio, gpio_function_t fn)
{

}

uint uart_init(uart_inst_t* uart, uint baudrate)
{
    return 1;
}

void uart_set_hw_flow(uart_inst_t* uart, bool cts, bool rts)
{

}

void uart_set_fifo_enabled(uart_inst_t* uart, bool enabled)
{

}

void uart_set_irqs_enabled(uart_inst_t* uart, bool rx_has_data, bool tx_needs_data)
{

}

bool uart_is_writable(uart_inst_t* uart)
{
    return true;
}

void uart_puts(uart_inst_t* uart, const char* s)
{

}

void uart_putc_raw(uart_inst_t* uart, char c)
{

}

char uart_getc(uart_inst_t* uart)
{
    return 0;
}

bool uart_is_readable(uart_inst_t* uart)
{
    return true;
}

void irq_set_enabled(uint num, bool enabled)
{

}

void irq_set_exclusive_handler(uint num, irq_handler_t handler)
{

}
