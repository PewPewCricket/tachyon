//
// Created by pewpewcricket on 8/16/25.
//

#ifndef UTIL_H
#define UTIL_H
#include <stddef.h>

void* memmove(void* dest, const void* src, const size_t n);
int strcmp(const char *s1, const char *s2);
char* itoa(int num, char* str, const int base);
char* utoa(unsigned int num, char* str, const int base);
_Noreturn void die();

extern void _die();

#endif //UTIL_H
