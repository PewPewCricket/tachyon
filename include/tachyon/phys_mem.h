#pragma once
#include <stdint.h>

#define MAX_ZONE_REGIONS 256

enum page_flags {
	PG_RESERVED		= 1 << 0,
	PG_DIRTY		= 1 << 1,
	PG_LOCKED		= 1 << 2,
};

enum mem_type {
	MEM_DMA,
	MEM_DMA32,
	MEM_NORMAL,
	MEM_DEVICE,
};

struct page {
	enum page_flags flags;
	uint64_t pfn;
	uint16_t refcount;
	uint8_t order;
};

struct buddy_freelist {
	struct buddy_freelist *next;
	struct buddy_freelist *last;
	struct page *page;
};

struct region {
	enum mem_type type;
	uint64_t length;
	struct page *map;
	struct buddy_freelist *freelist;
};

struct zone {
	enum mem_type type;
	uint64_t region_count;
	struct region *regions[MAX_ZONE_REGIONS];
};

#ifndef PMEM_H_NO_MEM_MAP
extern struct page *mem_map;
#endif