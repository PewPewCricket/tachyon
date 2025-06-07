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

__attribute__((aligned(0x10))) 
static struct idtEntry idt[256];
static struct idtr idtr;

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    struct idtEntry* descriptor = &idt[vector];

    descriptor->isr_low        = (uint32_t)isr & 0xFFFF;
    descriptor->segment        = 0x08;
    descriptor->attributes     = flags;
    descriptor->isr_high       = (uint32_t)isr >> 16;
    descriptor->reserved       = 0;
}

__attribute__((noreturn))
void exception_handler(uint32_t n) {
    __asm__ volatile ("cli; hlt"); // Completely hangs the computer
    __builtin_unreachable();
}

extern uint32_t* isr_stub_table[];

int init_idt() {
    for (int i = 0; i < 32; i++) {
        idt_set_descriptor(i, isr_stub_table[i], 0x8E);
    }

    idtr.base = (uint32_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(struct idtEntry) * 256 - 1;

    __asm__ volatile ("lidt %0" : : "m"(idtr));
    // __asm__ volatile ("sti"); // Enable interupts

    return 0;
}