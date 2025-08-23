#ifndef MMAP_H
#define MMAP_H
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>

#define MSR_EFER 0xC0000080

#define PAGE_SIZE 4096
#define PAGE_ALIGN _Alignas(PAGE_SIZE)

#define PTE_PRESENT       (1ULL << 0)
#define PTE_WRITABLE      (1ULL << 1)
#define PTE_USER          (1ULL << 2)
#define PTE_WRITE_THROUGH (1ULL << 3)
#define PTE_CACHE_DISABLE (1ULL << 4)
#define PTE_ACCESSED      (1ULL << 5)
#define PTE_DIRTY         (1ULL << 6)
#define PTE_PAT           (1ULL << 7)
#define PTE_SIZE		  (1ULL << 7)
#define PTE_GLOBAL        (1ULL << 8)
#define PTE_NX            (1ULL << 63)

static inline uint64_t mkpte(const uint64_t addr, const uint64_t flags) {
	return (addr & 0x000FFFFFFFFFF000ULL) | flags;
}

void map_identity();
void enter_lm();

#endif //MMAP_H
