#ifndef UTIL_H
#define UTIL_H
#include <stdint.h>
#include <limits.h>

#define MASK(T, width) ((T)((width) >= (int)(sizeof(T) * CHAR_BIT) ? ~(T)0 : ((T)1 << (width)) - 1))
#define GET_BITS(T, n, a, b) ((T)(((uint64_t)(n) >> (a)) & MASK(T, b)))
#define SET_BITS(T, n, a, b) ((T)(((uint64_t)(n) & MASK(T, b)) << (a)))

_Noreturn void error(char *restrict fmt, ...);
_Noreturn void die();

extern _Noreturn void _die();

#endif //UTIL_H
