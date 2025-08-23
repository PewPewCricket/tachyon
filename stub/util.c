#include <stdarg.h>
#include <scrio.h>
#include <util.h>
#include <globals.h>

uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

_Noreturn void die() {
	fbprintf("!!! SYSTEM HALTED !!!\n");
	_die();
}

_Noreturn void error(char *restrict fmt, ...) {
	va_list args; va_start(args, fmt);
	fbsetcol(0xFF0000, 0x000000);
	fbputs("ERROR: ");
	fbsetcol(0xFFFFFF, 0x000000);
	vfbprintf(fmt, args);
	va_end(args);
	die();
}

_Noreturn void __stack_chk_fail() {
	error("Stack smashing detected!\n");
}