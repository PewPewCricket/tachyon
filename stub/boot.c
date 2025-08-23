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
}
