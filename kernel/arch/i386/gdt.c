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

#include <gdt.h>

int encodeGdtEntry(struct GDT* gdt, int base, int limit, uint8_t access, uint8_t flags) {
    if (limit < 0xFFFFF) return -1;

    gdt->baseLow = (uint16_t) base;
    gdt->baseMid = (uint8_t) base >> 16;
    gdt->baseHigh = (uint8_t) base >> 24;

    gdt->limitLow = (uint16_t) limit;
    gdt->limitHigh = (uint8_t) limit >> 16 & 0b1111;

    gdt->access = access;

    gdt->flags = flags & 0b1111;

    return 0;
}

void gdtInit(struct GDT* gdts, int n) {
    struct GDTR gdtr;
    gdtr.size = sizeof(struct GDT) * n - 1;
    gdtr.offset = (uint32_t)gdts;

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
}