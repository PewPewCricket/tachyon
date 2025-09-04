#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <lib/string.h>
#include <limine/limine.h>
#include <limine/types.h>
#include <limine/requests.h>
#include <drivers/fbcon.h>
#include <tachyon/printk.h>
#include <limine/requests.h>
#include <tachyon/mm/memory.h>

#include "tachyon/panic.h"

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

    mem_hhdm_offset = limine_hhdm_request.response->offset;

    const struct limine_framebuffer *framebuffer = limine_framebuffer_request.response->framebuffers[0];
    fbcon_setfb(framebuffer);
    printk(KERN_INFO, "Tachyon X.X.X\n");

    mm_init();

    printk(KERN_EMERG, "!!! system halted !!!\n");
    panic();
}