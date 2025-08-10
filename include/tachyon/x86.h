#ifndef X86_H
#define X86_H

struct far_ptr {
	uint16_t offset;
	uint16_t selector;
} __attribute__((packed));

static inline void hcf() {
	__asm__ __volatile__ (
		"cli\n"
		"1:\n"
		"hlt\n"
		"jmp 1b\n"
   );
}

#endif //X86_H
