#include <stdint.h>
#include "gdt.h"

void gdt_set_entry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags, struct gdt_entry* gdt) {
    gdt->base_low = (uint16_t) (base & 0xFFFF);
    gdt->base_middle = (uint8_t) (base & 0xFF0000);
    gdt->base_high = (uint8_t) (base & 0xFF000000);

    gdt->limit_low = (uint16_t) (limit & 0xFFFF);
    gdt->limit_high = (uint8_t) (limit & 0xF0000);

    gdt->access = access;
    gdt->flags = flags;
}

void lgdt(struct gdt_ptr* gdt_descriptor) {

}