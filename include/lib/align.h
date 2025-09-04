#pragma once

#define PAGE_SIZE 4096
#define PAGE_SHIFT 12

#define OFFSET_OF(type, member) \
	((size_t) &((type *)0)->member)
#define CONTAINER_OF(ptr, type, member) \
	((type *) ((char *) (ptr) - OFFSET_OF(type, member)))
#define ALIGN_UP(addr) \
	(((addr) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define ALIGN_DOWN(addr) \
	((addr) & ~(PAGE_SIZE - 1))
#define ADDR_TO_PFN(addr) \
	((uintptr_t) (addr) >> PAGE_SHIFT)
#define PFN_TO_ADDR(pfn) \
	((pfn) << PAGE_SHIFT)