#include "pico.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

//
// GPIO
//

void gpio_set_function(unsigned int gpio, gpio_function_t fn) {}

//
// UART
//

static int uart_fd = -1;
static irq_handler_t default_handler = NULL;

uint uart_init(uart_inst_t* uart, uint baudrate)
{
    uart_fd = posix_openpt(O_RDWR | O_NOCTTY);
    if (uart_fd < 0) {
        perror("posix_openpt");
        return 0;
    }

    if (grantpt(uart_fd) < 0) {
        perror("grantpt");
        return 0;
    }

    if (unlockpt(uart_fd) < 0) {
        perror("unlockpt");
        return 0;
    }

    char* slave_name = ptsname(uart_fd);
    if (!slave_name) {
        perror("ptsname");
        return 0;
    }

    int flags = fcntl(uart_fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        return 0;
    }
    if (fcntl(uart_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL O_NONBLOCK");
        return 0;
    }

    printf("UART serial port being emulated in %s\n", slave_name);

    return 1;
}

void uart_step()
{
    if (uart_fd != 1 && default_handler) {
        default_handler();
    }
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
    write(uart_fd, s, strlen(s));
}

void uart_putc_raw(uart_inst_t* uart, char c)
{
    write(uart_fd, &c, 1);
}

char uart_getc(uart_inst_t* uart)
{
    char ch;
    read(uart_fd, &ch, 1);
    return ch;
}

bool uart_is_readable(uart_inst_t* uart)
{
    fd_set readfds;
    struct timeval timeout;

    FD_ZERO(&readfds);
    FD_SET(uart_fd, &readfds);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;  // Non-blocking

    int result = select(uart_fd + 1, &readfds, NULL, NULL, &timeout);
    return result > 0 && FD_ISSET(uart_fd, &readfds);
}

//
// IRQ
//

void irq_set_enabled(uint num, bool enabled)
{

}

void irq_set_exclusive_handler(uint num, irq_handler_t handler)
{
    default_handler = handler;
}
