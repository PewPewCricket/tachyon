#include "include/memory.h"

#include <scrio.h>
#include <stddef.h>
#include <stdint.h>
#include <globals.h>
#include <memory.h>
#include <util.h>
#include <string.h>

#include "include/scrio.h"

PAGE_ALIGN uint64_t pml4[512];
PAGE_ALIGN uint64_t pdpt[512];
PAGE_ALIGN uint64_t pd[4][512];

void map_identity() {
	fbprintf("Identity mapping first 4GiB\n");
	pml4[0] = mkpte((uint64_t) (uint32_t) pdpt,  PTE_PRESENT | PTE_WRITABLE);

	for (int i = 0; i < 4; i++) {
		pdpt[i] = mkpte((uint64_t) (uint32_t) pd[i], PTE_PRESENT | PTE_WRITABLE);
	}

	for (int i = 0; i < 4; i++) {
		const uint64_t pdpt_base = (uint64_t) i * 0x40000000ULL;
		uint64_t addr = 0;
		for (int j = 0; j < 512; j++) {
			addr = pdpt_base + (uint64_t)j * 0x200000UL;
			pd[i][j] = mkpte(addr, PTE_PRESENT | PTE_WRITABLE| PTE_SIZE);
		}
		fbprintf("\t[%xl - %xl]: identity mapped.\n", pdpt_base, addr + 0x200000 - 1);
	}
}

void enter_lm() {
	fbprintf("Entering long mode... ");
	__asm__ __volatile__(
		"movl %%cr4, %%eax\n\t"
		"orl $0x20, %%eax\n\t"
		"movl %%eax, %%cr4\n\t"

		"movl %[pml4_addr], %%eax\n\t"
		"movl %%eax, %%cr3\n\t"

		"movl %[efer], %%ecx\n\t"
		"rdmsr\n\t"
		"orl $0x100, %%eax\n\t"
		"wrmsr\n\t"

		"movl %%cr0, %%eax\n\t"
		"orl $0x80000000, %%eax\n\t"
		"movl %%eax, %%cr0\n\t"
		:
		: [pml4_addr] "r"(pml4), [efer] "i"(MSR_EFER)
		: "eax", "ecx", "edx"
	);
	fbprintf("DONE!\n");
}