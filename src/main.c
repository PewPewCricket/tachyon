#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "limine.h"
#include "string.h"
#include "font.h"

// Set the base revision to 3, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .c file, as seen fit.

__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

static void hcf(void) {
    for (;;) {
        asm ("hlt");
    }
}

// Print n bytes of s to the framebuffer at (x, y) with a specific bg and fg color
void putsfb(char* s, uint32_t n, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg, struct limine_framebuffer *fb) {
    volatile uint32_t *fb_ptr = fb->address;
    for(uint32_t i = 0; i < n; i++){
        for(uint32_t j = 0; j < 16; j++) {
            for(uint32_t k = 0; k < 8; k++) {
                unsigned char bit = (font_vga8x16[s[i] * 16 + j] << k) & 0b10000000;
                if(bit) {
                    fb_ptr[(x * 8 + k) + ((y * 16 + j) * fb->width) + (8 * i)] = fg;
                } else {
                    fb_ptr[(x * 8 + k) + ((y * 16 + j) * fb->width) + (8 * i)] = bg;
                }
            }
        }
    }
}

void kmain(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    // Fetch the first framebuffer.
    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];

    putsfb("Hellorld", 8, 0, 0, 0xFFFFFF, 0x0000FF, fb);

    // We're done, just hang...
    hcf();
}