#ifndef STRING_H
#define STRING_H 1

#include <stddef.h>
#include <stdint.h>

void* memcpy(void *dest, const void *src, size_t n);
void* memset(void *s, int c, size_t n);
void* memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
int strlen(const char* s);
int strnlen(const char* s, size_t n);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);
char* strcpy(char* dest, const char* s);
char* strncpy(char* dest, const char* s, size_t n);

#endif