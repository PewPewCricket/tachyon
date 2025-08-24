#include <stdint.h>
#include <cpuid.h>
#include <globals.h>
#include <scrio.h>
#include <util.h>
#include <memory.h>
#include <elf.h>
#include <string.h>

uint64_t gdt[3] = {
	0,
	0x00AF9A000000FFFF,
	0x00AC92000000FFFF
};

struct gdtr {
	uint16_t size;
	uint64_t offset;
} __attribute__((packed));

typedef struct {
	uint32_t offset;
	uint16_t segment;
} __attribute__((packed)) far_ptr;

void boot() {
	// Check if we are on a 64-bit processor
	unsigned int eax, ebx, ecx, edx;
	if (!__get_cpuid(0x80000001, &eax, &ebx, &ecx, &edx))
		error("CPUID not supported.\n");
	if (!(edx & bit_LM))
		error("Processor is not 64 bits.\n");
	if (!(edx & (1 << 3)))
		error("Processor does not support 2MiB pages.\n");

	map_identity();
	enter_lm();

	auto const kernel_elf = (Elf64_Ehdr*) _kernel_ptr;
	auto const kernel_elf_entry = (uint64_t) kernel_elf->e_entry;
	auto kernel_start = NULL;
	for (int i = 0; i < kernel_elf->e_phnum; i++) {
		auto const phdr = (Elf64_Phdr*) (kernel_elf->e_phoff + _kernel_ptr) + (kernel_elf->e_phentsize * i);
		if (phdr->p_type != PT_LOAD) continue;
		if (phdr->p_vaddr == kernel_elf_entry)
			kernel_start = (void*) (phdr->p_offset + _kernel_ptr);
	}
	if (kernel_start == NULL) error("Kernel entry point could not be found!");
	fbprintf("Found kernel entry point at: 0x%x.\n", kernel_start);

	struct gdtr gdtr;
	gdtr.size = sizeof(uint64_t) * 3;
	gdtr.offset = (uint64_t) (uint32_t) gdt;

	__asm__ __volatile__ ("lgdt %0" : : "m"(gdtr));

	far_ptr far_ptr;
	far_ptr.offset = (uint32_t) kernel_start;
	far_ptr.segment = 0x08;
	fbprintf("Jumping to 64 bit kernel...");
	__asm__ __volatile__ (
		"movl $0x10, %%eax\n\t"
		"movw %%ax, %%ds\n\t"
		"movw %%ax, %%es\n\t"
		"movw %%ax, %%gs\n\t"
		"movw %%ax, %%ss\n\t"
		"ljmp *%[dest]\n\t"
		:
		: [dest] "m" (far_ptr)
		: "eax"
	);

	// big uh oh
	error("This should never print, if you see this, something went terribly wrong, please report this to the developer IMMEDIATELY!");
}