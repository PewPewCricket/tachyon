#include <elf.h>
#include <stdint.h>

extern char __reloc_start, __reloc_end, __kernel_base;
extern void _die(void);

__attribute__((no_stack_protector))
static inline uint64_t runtime_base(void) {
	uint64_t b;
	__asm__ __volatile__("lea __kernel_base(%%rip), %0" : "=r"(b));
	return b;
}

__attribute__((no_stack_protector))
void patch_relocs(void) {
	const uint64_t b = runtime_base();
	const uint64_t l = (uint64_t)&__kernel_base;

	const uintptr_t rela_base = (uintptr_t)b + ((uintptr_t)&__reloc_start - (uintptr_t)l);
	const uintptr_t rela_end = (uintptr_t)b + ((uintptr_t)&__reloc_end - (uintptr_t)l);

	for (Elf64_Rela *r = (Elf64_Rela *)rela_base; (uintptr_t)r < rela_end; r++) {
		switch (ELF64_R_TYPE(r->r_info)) {
			case R_X86_64_RELATIVE: {
				uint64_t *p = (uint64_t *)(b + r->r_offset);
				*p = b + r->r_addend;
				break;
			}
			default:
				_die();
		}
	}
}