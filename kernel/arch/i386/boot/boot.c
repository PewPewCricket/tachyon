/* Tachyon Kernel
 * Copyright (C) 2025 PewPewCricket
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdint.h>

#include <idt.h>
#include <video/framebuffer.h>
#include <multiboot2.h>
#include <drivers/serial.h>
#include <video/console.h>

void kmain();
int init_gdt();

void boot(uint32_t* mb2_data) {
    struct framebuffer boot_fb = {0};

    uint8_t* tag_ptr = (uint8_t*)mb2_data;
    uint32_t total_size = *(uint32_t*)tag_ptr;
    tag_ptr += 8; // Skip total_size and reserved

    while (tag_ptr < ((uint8_t*)mb2_data) + total_size) {
        uint32_t tag_type = *(uint32_t*)tag_ptr;
        uint32_t tag_size = *(uint32_t*)(tag_ptr + 4);

        if (tag_type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER) {
            struct multiboot_tag_framebuffer* fb_tag = (struct multiboot_tag_framebuffer*)tag_ptr;

            // Common fields
            boot_fb.address = (void*)(uintptr_t)fb_tag->common.framebuffer_addr;
            boot_fb.pitch   = fb_tag->common.framebuffer_pitch;
            boot_fb.width   = fb_tag->common.framebuffer_width;
            boot_fb.height  = fb_tag->common.framebuffer_height;
            boot_fb.bpp     = fb_tag->common.framebuffer_bpp;

            if (fb_tag->common.framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_RGB) {
                // RGB framebuffer
                boot_fb.red_field_pos   = fb_tag->framebuffer_red_field_position;
                boot_fb.red_mask_size   = fb_tag->framebuffer_red_mask_size;
                boot_fb.green_field_pos = fb_tag->framebuffer_green_field_position;
                boot_fb.green_mask_size = fb_tag->framebuffer_green_mask_size;
                boot_fb.blue_field_pos  = fb_tag->framebuffer_blue_field_position;
                boot_fb.blue_mask_size  = fb_tag->framebuffer_blue_mask_size;
            } else if (fb_tag->common.framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED) {
                // Indexed (palette) framebuffer
                // TODO: Handle palette framebuffer
            } else if (fb_tag->common.framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT) {
                // EGA text framebuffer
                // TODO: Handle EGA text framebuffer
            }
        }

        // Tags are aligned to 8 bytes
        tag_ptr += (tag_size + 7) & ~7;
    }

    init_gdt();
    init_idt();
    // Setup APIC here, serial will die without it

    serial_init();
    kcon_init(&boot_fb, 0);
    kcon_set_mode(KCON_BOTH);
    kmain();
}