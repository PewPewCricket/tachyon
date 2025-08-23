#ifndef UTIL_H
#define UTIL_H
#include <stdint.h>
#include <limits.h>

_Noreturn void error(char *restrict fmt, ...);
_Noreturn void die();

extern _Noreturn void _die();

#endif //UTIL_H
