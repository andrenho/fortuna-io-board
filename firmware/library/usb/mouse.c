#include "mouse.h"

#include "fortuna.h"

static uint8_t device_id;
static uint8_t instance;

void mouse_init(uint8_t device_id_, uint8_t instance_)
{
    device_id = device_id_;
    instance = instance_;
    tuh_hid_set_protocol(device_id, instance, HID_PROTOCOL_REPORT);
}

void mouse_process_report(hid_mouse_report_t const* report)
{
    Event e = {
        .type = E_MOUSE,
        .mouse = (MouseEvent) {
            .x = report->x,
            .y = report->y,
            .wheel = report->wheel,
            .buttons = report->buttons,
        }
    };
    fortuna_add_event(&e);
    vga_move_pointer(report->x, report->y);
}
