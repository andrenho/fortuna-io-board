#include <stddef.h>

#include "fortuna.h"

int main()
{
    fortuna_init(DEFAULT_QUEUE_SIZE, NULL);

    fb_set_pixel(vga_framebuffer(), 10, 10, C_WHITE);

    for (;;) {
        Event e;
        while (fortuna_next_event(&e)) {

        }
    }

}