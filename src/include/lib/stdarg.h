#ifndef STDARG_H
#define STDARG_H 1

#include <stddef.h>

typedef void* va_list;

// Assuming x86-64 architecture and calling convention
#define va_start(ap, last) ap = (va_list)(((char*)&last) + sizeof(last))
#define va_arg(ap, type)   (*(type*)((ap += sizeof(type)) - sizeof(type)))
#define va_end(ap)         (void)(ap = NULL)

#endif