#include "vga.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "color.h"
#include "framebuffer.h"

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/dma.h"

#include "hsync.pio.h"
#include "vsync.pio.h"
#include "rgb640.pio.h"
#include "rgb320.pio.h"

typedef enum {HSYNC=16, VSYNC, LO_GRN, HI_GRN, BLUE_PIN, RED_PIN} VgaPins;

// VGA framebuffer
static void* address_pointer = NULL;

// framebuffer
static Framebuffer* fb = NULL;

// DMA channels and PIO program offsets
static int rgb_chan_0, rgb_chan_1;
static uint rgb_offset, vsync_offset, hsync_offset;
static pio_program_t const* current_rgb_program = nullptr;

// current cannon location
static uint16_t current_scanline = 0;
static uint8_t  current_framebuffer = 0;
static volatile bool new_vsync = false;

// state machines
static const uint HSYNC_SM = 0;
static const uint VSYNC_SM = 1;
static const uint RGB_SM = 2;

// mouse location
static constexpr uint8_t CURSOR_HEIGHT = 8;
static int8_t move_mouse_x = 0, move_mouse_y = 0;
static uint16_t mouse_x = 0, mouse_y = 0;
static uint8_t vga_data_array_mouse[640 * CURSOR_HEIGHT / 2];
static bool show_mouse_pointer = false;

// VGA timing constants
#define H_ACTIVE   655    // (active + frontporch - 1) - one cycle delay for mov
#define V_ACTIVE   479    // (active - 1)
#define RGB_ACTIVE 319    // (horizontal active)/2 - 1
// #define RGB_ACTIVE 639 // change to this if 1 pixel/byte

// Pixel color array that is DMA's to the PIO machines and
// a pointer to the ADDRESS of this color array.
// Note that this array is automatically initialized to all 0's (black)
// Bit masks for drawPixel routine
#define TOPMASK 0b00001111
#define BOTTOMMASK 0b11110000

static const uint16_t __in_flash() mouse_pointer[] = {
    0b1111101010101010,
    0b1100111010101010,
    0b1100001110101010,
    0b1100000011101010,
    0b1100000000111010,
    0b1100001111101010,
    0b1100111010101010,
    0b1111101010101010,
};


static void dma_handler()   // DMA handler is called at the end of each HSYNC
{
    // Clear the interrupt request for DMA control channel
    dma_hw->ints0 = (1u << rgb_chan_0);

    // increment scanline (1..)
    current_scanline++;                  // new current scanline
    if (current_scanline >= 480) {       // last scanline?
        current_scanline = 0;            // restart scanline

        /* TODO
        if (current_mode == V_SPRITES)
            current_framebuffer = (current_framebuffer == 1) ? 2 : 1;
        */

        new_vsync = true;
    }

    int16_t mouse_diff = current_scanline / (fb->h == 240 ? 2 : 1) - (int) mouse_y;
    if (show_mouse_pointer && mouse_diff >= 0 && mouse_diff < CURSOR_HEIGHT) {
        address_pointer = &vga_data_array_mouse[mouse_diff * (fb->w >> 1)];
    } else {
        address_pointer = &fb->data[fb_pixel_idx(fb, 0, current_scanline)];
    }
}

static void init_dma()
{
    // Channel Zero (sends color data to PIO VGA machine)
    dma_channel_config c0 = dma_channel_get_default_config(rgb_chan_0);  // default configs
    channel_config_set_transfer_data_size(&c0, DMA_SIZE_8);              // 8-bit txfers
    channel_config_set_read_increment(&c0, true);                        // yes read incrementing
    channel_config_set_write_increment(&c0, false);                      // no write incrementing
    channel_config_set_dreq(&c0, DREQ_PIO0_TX2) ;                        // DREQ_PIO0_TX2 pacing (FIFO)
    channel_config_set_chain_to(&c0, rgb_chan_1);                        // chain to other channel

    dma_channel_configure(
        rgb_chan_0,                 // Channel to be configured
        &c0,                        // The configuration we just created
        &pio0->txf[RGB_SM],         // write address (RGB PIO TX FIFO)
        &fb->data,                  // The initial read address (pixel color array)
        fb->w / 2,                  // Number of transfers; in this case each is 1 byte.
        false                       // Don't start immediately.
    );

    // Channel One (reconfigures the first channel)
    dma_channel_config c1 = dma_channel_get_default_config(rgb_chan_1);   // default configs
    channel_config_set_transfer_data_size(&c1, DMA_SIZE_32);              // 32-bit txfers
    channel_config_set_read_increment(&c1, false);                        // no read incrementing
    channel_config_set_write_increment(&c1, false);                       // no write incrementing
    channel_config_set_chain_to(&c1, rgb_chan_0);                         // chain to other channel

    dma_channel_configure(
        rgb_chan_1,                         // Channel to be configured
        &c1,                                // The configuration we just created
        &dma_hw->ch[rgb_chan_0].read_addr,  // Write address (channel 0 read address)
        &address_pointer,                   // Read address (POINTER TO AN ADDRESS)
        1,                                  // Number of transfers, in this case each is 4 byte
        false                               // Don't start immediately.
    );

    // enable DMA
    dma_channel_set_irq0_enabled(rgb_chan_0, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);
    irq_set_priority(DMA_IRQ_0, 0);

    // start DMA channel
    dma_start_channel_mask((1u << rgb_chan_0)) ;
}


static void initialize_pio()
{
    // load PIO programs
    hsync_offset = pio_add_program(pio0, &hsync_program);
    vsync_offset = pio_add_program(pio0, &vsync_program);
    rgb_offset = pio_add_program(pio0, &rgb640_program);
    current_rgb_program = &rgb640_program;

    // initialize PIO programs
    hsync_program_init(pio0, HSYNC_SM, hsync_offset, HSYNC);
    vsync_program_init(pio0, VSYNC_SM, vsync_offset, VSYNC);
    rgb640_program_init(pio0, RGB_SM, rgb_offset, LO_GRN);

    // initialize PIO state machine counters.
    pio_sm_put_blocking(pio0, HSYNC_SM, H_ACTIVE);
    pio_sm_put_blocking(pio0, VSYNC_SM, V_ACTIVE);
    pio_sm_put_blocking(pio0, RGB_SM, (fb->w / 2) - 1);

    // claim DMA channels
    rgb_chan_0 = dma_claim_unused_channel(true);
    rgb_chan_1 = dma_claim_unused_channel(true);

    // initialize DMA
    init_dma();

    // initialize PIO programs in sync
    pio_enable_sm_mask_in_sync(pio0, ((1u << HSYNC_SM) | (1u << VSYNC_SM) | (1u << RGB_SM)));
}


static void draw_mouse_pointer(uint16_t x, uint16_t y, uint8_t color)
{
    const int pixel = ((fb->w * y) + x);
    if (pixel & 1)
        vga_data_array_mouse[pixel>>1] = (vga_data_array_mouse[pixel>>1] & TOPMASK) | (color << 4) ;
    else
        vga_data_array_mouse[pixel>>1] = (vga_data_array_mouse[pixel>>1] & BOTTOMMASK) | color;
}


static void update_mouse_pointer()
{
    if (show_mouse_pointer) {
        mouse_x = MIN(MAX(mouse_x + move_mouse_x, 0), fb->w - 1);
        mouse_y = MIN(MAX(mouse_y + move_mouse_y, 0), fb->h - 1);
        move_mouse_x = 0;
        move_mouse_y = 0;

        // copy mouse lines onto mouse buffer
        memcpy(vga_data_array_mouse, &fb->data[fb_pixel_idx(fb, 0, mouse_y * (fb->h == 240 ? 2 : 1))], (CURSOR_HEIGHT * fb->w ) >> 1);

        // add mouse to mouse array
        for (uint8_t x = 0; x < 8; ++x) {
            for (uint8_t y = 0; y < 8; ++y) {
                uint8_t color = (mouse_pointer[y] >> (14 - (x * 2))) & 0b11;
                if (color == 0b11)
                    draw_mouse_pointer(mouse_x + x, y, C_BLACK);
                else if (color == 0b00)
                    draw_mouse_pointer(mouse_x + x, y, C_WHITE);
            }
        }
    }
}


static void copy_sprites_vsync()
{
    /* TODO
    if (current_mode == V_SPRITES) {
        // copy framebuffer 0 on top of current framebuffer
        uint8_t opposite_framebuffer = (current_framebuffer == 1) ? 2 : 1;
        memcpy(&data_array[pixel_idx(0, 0, opposite_framebuffer)], &data_array[0], FRAMEBUFFER_SZ);

        // add sprites
        for (uint16_t i = 0; i < sprite_sz; ++i)
            fb::draw_image(*sprites[i].image, sprites[i].x, sprites[i].y, opposite_framebuffer);
    }
    */
}


void vga_init()
{
    fb = fb_new(640, 480);
    address_pointer = &fb->data[0];

    initialize_pio();

    printf("VGA initialized.\n");
}

void vga_step()
{
    if (new_vsync) {
        update_mouse_pointer();
        copy_sprites_vsync();

        new_vsync = false;
    }
}

struct Framebuffer* vga_framebuffer()
{
    return fb;
}

int vga_width()
{
    return fb->w;
}

int vga_height()
{
    return fb->h;
}

void vga_show_pointer(bool v)
{
    show_mouse_pointer = v;
}

void vga_set_pointer(uint16_t x, uint16_t y)
{
    mouse_x = x;
    mouse_y = y;
}

void vga_move_pointer(int8_t x, int8_t y)
{
    move_mouse_x = x;
    move_mouse_y = y;
}
