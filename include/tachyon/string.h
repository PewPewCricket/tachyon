#ifndef STRING_H
#define STRING_H
#include <stddef.h>

int strlen(const char* s);
int strnlen(const char* s, size_t n);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);
char* strcpy(char* dest, const char* s);
char* strncpy(char* dest, const char* s, size_t n);

#endif //STRING_H
