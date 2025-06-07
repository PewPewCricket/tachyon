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
#include <stdbool.h>

struct gdtr {
    uint16_t size;
    uint32_t offset;
} __attribute__((packed));

int encode_gdt_entry(uint64_t* entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);

static uint64_t gdt[6];

int encode_gdt_entry(uint64_t* entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags) {
    if (limit > 0xFFFFF) return -1;
    uint8_t* target = (uint8_t*) entry;

    // Encode the limit
    target[0] = limit & 0xFF;
    target[1] = (limit >> 8) & 0xFF;
    target[6] = (limit >> 16) & 0x0F;
    
    // Encode the base
    target[2] = base & 0xFF;
    target[3] = (base >> 8) & 0xFF;
    target[4] = (base >> 16) & 0xFF;
    target[7] = (base >> 24) & 0xFF;
    
    // Encode the access byte
    target[5] = access;
    
    // Encode the flags
    target[6] |= (flags << 4);

    return 0;
}

int init_gdt() {
    if (encode_gdt_entry(&gdt[0], 0, 0, 0, 0) < 0)            return -1;  // Null Desc.
    if (encode_gdt_entry(&gdt[1], 0, 0xFFFFF, 0x9A, 0xC) < 0) return -1;  // Kernel Code
    if (encode_gdt_entry(&gdt[2], 0, 0xFFFFF, 0x92, 0xC) < 0) return -1;  // Kernel Data
    if (encode_gdt_entry(&gdt[3], 0, 0xFFFFF, 0xFA, 0xC) < 0) return -1;  // User Code
    if (encode_gdt_entry(&gdt[4], 0, 0xFFFFF, 0xF2, 0xC) < 0) return -1;  // User Data

    struct gdtr gdtr;
    gdtr.size = sizeof(uint64_t) * 5;
    gdtr.offset = (uint32_t)gdt;

    // Load the GDT
    asm volatile ("lgdt %0" : : "m"(gdtr));

    // Reload segment registers
    asm volatile (
        "mov $0x10, %%ax\n" // 0x10 = offset of kernel data segment
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n"
        "ljmp $0x08, $next\n" // 0x08 = offset of kernel code segment
        "next:\n"
        : : : "memory", "ax"
    );

    return 0;
}