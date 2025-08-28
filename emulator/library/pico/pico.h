#ifndef PICO_H_
#define PICO_H_

#include <SDL3/SDL.h>

#define __in_flash()

typedef unsigned int uint;

//
// GPIO
//

typedef enum { GPIO_FUNC_UART } gpio_function_t;

void gpio_set_function (unsigned int gpio, gpio_function_t fn);

//
// UART
//

typedef void uart_inst_t;
static uart_inst_t* uart0 = NULL;

#define UART_FUNCSEL_NUM(a, b) GPIO_FUNC_UART

uint uart_init (uart_inst_t *uart, uint baudrate);
void uart_set_hw_flow (uart_inst_t *uart, bool cts, bool rts);
void uart_set_fifo_enabled (uart_inst_t *uart, bool enabled);
void uart_set_irqs_enabled (uart_inst_t * uart, bool rx_has_data, bool tx_needs_data);
bool uart_is_writable (uart_inst_t *uart);
void uart_puts (uart_inst_t *uart, const char *s);
void uart_putc_raw (uart_inst_t *uart, char c);
char uart_getc (uart_inst_t *uart);
bool uart_is_readable (uart_inst_t *uart);

//
// IRQ
//

#define UART0_IRQ 33

typedef void(* irq_handler_t) (void);

void irq_set_enabled (uint num, bool enabled);
void irq_set_exclusive_handler (uint num, irq_handler_t handler);

#endif