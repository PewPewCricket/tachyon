#pragma once

#include <stdint.h>

struct idtEntry{
	uint16_t    isr_low;      // The lower 16 bits of the ISR's address
	uint16_t    segment;      // The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t     reserved;     // Set to zero
	uint8_t     attributes;   // Type and attributes; see the IDT page
	uint16_t    isr_high;     // The higher 16 bits of the ISR's address
} __attribute__((packed));

struct idtr{
	uint16_t	limit;
	uint32_t	base;
} __attribute__((packed));

int init_idt();