#include <stdint.h>

void breakpoint_func() {
    return;
}

void isr_exception(uint32_t err) {
    switch(err) {
        // Breakpoint
        case 0x3:
            breakpoint_func();
            break;
        default:
            asm("cli");
            for (;;) {
                asm("hlt");
            }
    }

    return;
}