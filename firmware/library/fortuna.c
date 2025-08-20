#include "fortuna.h"

#include <stdio.h>

#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <pico/util/queue.h>

static uint16_t    event_queue_size;
static queue_t     event_queue;
static semaphore_t semaphore;

static void (*core1_step_function)();

static void core1_entry();

void fortuna_init(uint16_t event_queue_size_, void(* core1_step_function_)())
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
    printf("==============================================================\n");

    vga_init();

    /* TODO
    rtc::init();
    usb::init();
    user::init();
    vga::init();
    external::init();
    audio::init();
    */

    sem_release(&semaphore);

    // TODO user::init_interupts();

    for (;;) {
        vga_step();
        /* TODO
        usb::step();
        */
        if (core1_step_function)
            core1_step_function();
        else
            __wfi();
    }
}


bool fortuna_add_event(Event const* event)
{
    return queue_try_add(&event_queue, &event);
}

bool fortuna_next_event(Event* event)
{
    return queue_try_remove(&event_queue, event);
}