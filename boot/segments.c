#include <stdint.h>
#include <stdarg.h>
#include <tachyon/x86.h>

static void gdtr_create(uint64_t *gdt, uint8_t *gdtr, const unsigned int descriptors) {
	const uint16_t size = descriptors * sizeof(uint64_t) - 1;
	const uint32_t addr = (uint32_t) gdt;

	gdtr[0] = size & 0xFF;
	gdtr[1] = (size >> 8) & 0xFF;

	gdtr[2] = addr & 0xFF;
	gdtr[3] = (addr >> 8) & 0xFF;
	gdtr[4] = (addr >> 16) & 0xFF;
	gdtr[5] = (addr >> 24) & 0xFF;
}

uint64_t gdt_descriptor_create(const uint32_t base, const uint32_t limit, const uint16_t flag) {
	uint64_t descriptor;

	// Create the high 32 bit segment
	descriptor  =  limit       & 0x000F0000; // set limit bits 19:16
	descriptor |= (flag <<  8) & 0x00F0FF00; // set type, p, dpl, s, g, d/b, l and avl fields
	descriptor |= (base >> 16) & 0x000000FF; // set base bits 23:16
	descriptor |=  base        & 0xFF000000; // set base bits 31:24

	// Shift by 32 to allow for low part of segment
	descriptor <<= 32;

	// Create the low 32 bit segment
	descriptor |= base  << 16; // set base bits 15:0
	descriptor |= limit  & 0x0000FFFF; // set limit bits 15:0

	return descriptor;
}

void gdt_populate(uint64_t *gdt, const unsigned int descriptors, ...) {
	va_list args;
	va_start(args, descriptors);
	for (unsigned int i = 0; i < descriptors; i++) {
		const uint64_t descriptor = va_arg(args, uint64_t);
		gdt[i] = descriptor;
	}
	va_end(args);
}

// NOTE: we assume that the 2nd desc. is the code segment and the 3rd is the data segment.
void gdt_load(uint64_t *gdt, const unsigned int descriptors) {
	uint8_t gdtr[6];
	gdtr_create(gdt, gdtr, descriptors);
	__asm__ volatile ("lgdt (%0)" : : "r" (gdtr));

	__asm__ volatile (
		"mov $0x10, %%ax\n"
		"mov %%ax, %%ds\n"
		"mov %%ax, %%es\n"
		"mov %%ax, %%fs\n"
		"mov %%ax, %%gs\n"
		"mov %%ax, %%ss\n"
		"jmp $0x08, $.flush\n"
		".flush:\n"
		: : : "ax"
	);
}