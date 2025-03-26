#ifndef GDT_H
#define GDT_H 1

#include <stdint.h>
#include <stdbool.h>

// An entry in the GDT
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    unsigned int limit_high : 4;
    unsigned int flags : 4;
    uint8_t base_high;
} __attribute__((packed));

// GDT pointer structure
struct gdtr {
    uint16_t limit; // Size of the GDT - 1
    uint64_t base;  // Address of the GDT
} __attribute__((packed));

void gdt_set_entry(uint32_t base, uint32_t limit, uint8_t access, uint8_t flags, struct gdt_entry* gdt);
void lgdt(struct gdtr* gdtr);

#endif