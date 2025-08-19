#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stddef.h>

#define tostring(num, buffer, base) _Generic((num), \
signed char:          strtoll, \
unsigned char:        strtoull, \
signed short:         strtoll, \
unsigned short:       strtoull, \
signed int:           strtoll, \
unsigned int:         strtoull, \
signed long:          strtoll, \
unsigned long:        strtoull, \
signed long long:     strtoll, \
unsigned long long:   strtoull  \
)(num, buffer, base)

void *memcpy(void *restrict dest, const void *restrict src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

char* strtoll(int64_t num, char* buffer, const int base);
char* strtoull(uint64_t num, char* buffer, const int base);
int strcmp(const char *s1, const char *s2);

#endif //STRING_H
