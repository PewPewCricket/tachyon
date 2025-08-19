#include <stdarg.h>
#include <scrio.h>
#include <util.h>

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