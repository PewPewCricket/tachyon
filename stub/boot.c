#include <stdint.h>
#include <cpuid.h>
#include <globals.h>
#include <scrio.h>
#include <util.h>
#include <mmap.h>

void boot() {
	// Check if we are on a 64-bit processor
	unsigned int eax, ebx, ecx, edx;
	if (!__get_cpuid(0x80000001, &eax, &ebx, &ecx, &edx))
		error("CPUID not supported.\n");
	if (!(edx & 1 << 29))
		error("Processor is not 64 bits.\n");

	fbprintf("Refactoring is (not) fun!\n");
}
