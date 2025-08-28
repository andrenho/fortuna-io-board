#include "fortuna.h"

#include <stdio.h>
#include <malloc.h>

#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/util/queue.h>

static uint16_t    event_queue_size;
static queue_t     event_queue;
static semaphore_t semaphore;

static void (*core1_step_function)();

static void core1_entry();

void fortuna_init(uint16_t event_queue_size_, void(* core1_step_function_)(), int argc, char* argv[])
{
    event_queue_size = event_queue_size_;
    core1_step_function = core1_step_function_;

    // initialize event queue
    queue_init(&event_queue, sizeof(Event), event_queue_size);

    // wait for CORE 1 initialization
    sem_init(&semaphore, 0, 1);
    multicore_launch_core1(core1_entry);
    sem_acquire_blocking(&semaphore);

    // TODO - continue initialization
}

static void core1_entry()
{
    stdio_uart_init();

    vga_init();
    usb_init();
    panel_init();
    rtc_init();
    audio_init();

    sem_release(&semaphore);

    panel_init_interupts();

    for (;;) {
        vga_step();
        usb_step();
        if (core1_step_function)
            core1_step_function();
        else
            __wfi();
    }
}


bool fortuna_add_event(Event const* event)
{
    return queue_try_add(&event_queue, event);
}

bool fortuna_next_event(Event* event)
{
    return queue_try_remove(&event_queue, event);
}

uint32_t fortuna_total_ram()
{
    extern char __StackLimit, __bss_end__;
    return &__StackLimit  - &__bss_end__;
}

uint32_t fortuna_free_ram()
{
    struct mallinfo m = mallinfo();
    return fortuna_total_ram() - m.uordblks;
}

