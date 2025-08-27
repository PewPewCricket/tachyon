#pragma once
#include <stddef.h>
#include <stdint.h>

void *memcpy(void *restrict dest, const void *restrict src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
char* strtoll(int64_t num, char *buf, const int base) ;
char* strtoull(uint64_t num, char *buf, const int base);