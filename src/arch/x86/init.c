#include <stdint.h>

#include <kernel/panic.h>
#include <boot_info.h>

void gdt_init();
void idt_init();

void mb2_parse_info(uint32_t* mb2_addr, struct boot_info* mb2_info) {
    uint32_t* tag = mb2_addr + 2; // Skip header

    for (;;) {
        uint32_t type = tag[0];
        uint32_t size = tag[1];

        // Break out of loop at end tag.
        if (type == 0 && size == 8) break;

        switch (type) {
            case 8: { /* Framebuffer */
                uint32_t* addr = (uint32_t*) tag[3]; // Lower half of the u64, add checks that u64 isnt over 32 bit limit. 
                uint32_t pitch = tag[4];
                uint32_t width = tag[5];
                uint32_t height = tag[6];
                uint8_t depth = *((uint8_t*)(tag + 7));

                if (depth != 32) {
                    mb2_info->fb.addr = 0;
                    break;
                }

                mb2_info->fb.addr = (uint32_t*) addr;
                mb2_info->fb.pitch = pitch;
                mb2_info->fb.width = width;
                mb2_info->fb.height = height;
                mb2_info->fb.depth = (uint32_t) depth;

                break;
            }
            case 2: { /* Bootloader name */
                mb2_info->loader_name = (char*) (tag + 2);
            }
        }

        // Move to the next tag (align to 8 bytes)
        tag = (uint32_t*)((uintptr_t)tag + ((size + 7) & ~7));
    }
}

void kinit(uint32_t mb2_magic, uint32_t* mb2_addr) {
    if (!(mb2_magic == 0x36d76289)) {
        hcf();
    }

    struct boot_info boot_info;
    mb2_parse_info(mb2_addr, &boot_info);

    

    gdt_init();
    idt_init();

    return;
}