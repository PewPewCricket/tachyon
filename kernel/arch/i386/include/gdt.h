#pragma once

#include <stdint.h>

struct GDTR {
    uint16_t size;
    uint32_t offset;
} __attribute__((packed));

int encode_gdt_entry(uint64_t* entry, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
int init_gdt();