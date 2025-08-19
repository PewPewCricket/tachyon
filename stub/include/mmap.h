#ifndef MMAP_H
#define MMAP_H
#include <stdalign.h>
#include <stddef.h>

#define PAGE_SIZE 4096
#define PAGE_ALIGN _Alignas(PAGE_SIZE)

void mmap(const size_t stub_size, const size_t kernel_size);

#endif //MMAP_H
