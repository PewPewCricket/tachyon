#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include <string.h>
#include "fbio.h"
#include "gdt.h"

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

    putsfb("Nerve 1.0.0.0:dev-26.03.2025", 0, 0, 0xFFFFFF, 0x000000, fb);

    // Set up the GDT
    struct gdt_entry gdt[5];
    struct gdtr gdtr;

    gdt_set_entry(0x0, 0x0, 0x0, 0x0, &gdt[0]);             // Null Descriptor
    gdt_set_entry(0x0, 0xFFFFFFFF, 0x9A, 0xA, &gdt[1]);     // Kernel Code Segment
    gdt_set_entry(0x0, 0xFFFFFFFF, 0x92, 0xC, &gdt[2]);     // Kernel Data Segment
    gdt_set_entry(0x0, 0xFFFFFFFF, 0xFA, 0xA, &gdt[3]);     // User Mode Code Segment
    gdt_set_entry(0x0, 0xFFFFFFFF, 0xF2, 0xC, &gdt[4]);     // User Mode Data Segment
    // TSS GOES HERE! HAS DIFF. STRUCTURE THAN OTHER ENTRIES!!!

    lgdt(&gdtr);

    // We're done, just hang...
    hcf();
}