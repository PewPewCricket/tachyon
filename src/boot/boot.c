#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <lib/string.h>
#include <limine/limine.h>
#include <limine/types.h>
#include <tachyon/bootreqs.h>
#include <drivers/fbcon.h>
#include <tachyon/printk.h>
#include <tachyon/memory.h>

static volatile __limine_requests_start LIMINE_REQUESTS_START_MARKER;
static volatile __limine_requests_end LIMINE_REQUESTS_END_MARKER;
static volatile __limine_request LIMINE_BASE_REVISION(3);

static void hcf(void) {
    for (;;)
        asm ("hlt");
}

void boot(void) {
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    if (limine_framebuffer_request.response == NULL
    || limine_framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    const struct limine_framebuffer *framebuffer = limine_framebuffer_request.response->framebuffers[0];
    fbcon_setfb(framebuffer);
    printk(KERN_INFO, "Tachyon X.X.X\n");


    init_mm();

    hcf();
}