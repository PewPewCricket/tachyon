#include <stdint.h>
#include "panic.h"

void gdt_init();
void idt_init();

struct fb {
    uint32_t addr;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t pitch;
    uint32_t type;
};

struct fb fb;

void kinit(uint32_t mb2_magic, uint32_t* mb2_addr) {
    if (!(mb2_magic == 0x36d76289)) {
        hcf();
    }

    for (uint32_t i = 0;; i++) {
        if (mb2_addr[i] == 0 && mb2_addr[i + 1] == 8) {
            asm("int $0x03");
            break;
        }
    }

    gdt_init();
    idt_init();

    return;
}