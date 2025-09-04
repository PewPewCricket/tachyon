#pragma once
#include <lib/llist.h>
#include <stdint.h>

enum page_flags {
	PG_RESERVED = 0x1,
	/* Nothing here much for now... */
};

struct page {
	// Generic page information
	enum page_flags flags;
	struct region *region;
	uint32_t refcount; // TODO: change this to atomic_t!
	uintptr_t pfn;

	// Buddy allocator metadata
	struct list_head lru;
	uint8_t order;
};

struct region {
	struct page *map;
	uintptr_t length;
};

struct free_area {
	struct list_head *free_list;
	uintptr_t nr_free;
};