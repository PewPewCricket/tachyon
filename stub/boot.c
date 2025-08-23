#include <stdint.h>
#include <cpuid.h>
#include <globals.h>
#include <scrio.h>
#include <util.h>
#include <memory.h>

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

	fbprintf("Hello compatability mode!\nHello Tachyon!\n");
	const int t = 5;
	fbprintf("testing stack vars: %d\n", t);
	fbprintf("testing stack vars: %d\n", t);

	// Testing stack smash function... (broken as of now?)
	extern _Noreturn void __stack_chk_fail();
	__stack_chk_fail();
}