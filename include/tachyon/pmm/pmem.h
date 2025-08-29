#pragma once
#include <stdint.h>
#include <stddef.h>

enum page_flags {
	PG_RESERVED		= 1 << 0,
	PG_DIRTY		= 1 << 1,
	PG_LOCKED		= 1 << 2,
};

struct page {
	enum page_flags flags;
	uint64_t pfn;
	uint16_t refcount;
	signed int order;
};

struct buddy_freelist {
	struct buddy_freelist *next;
	struct buddy_freelist *prev;
	struct page *page;
};

struct region {
	uint64_t length;
	struct page *map;
	struct buddy_freelist *freelist_head;
	struct buddy_freelist *freelist_tail;
};

extern struct page *pmm_map;
extern struct region *pmm_regions;

void pmm_init();
uint64_t pmm_get_total_pages(uint64_t *region_count);
void pmm_print_freelist(const struct region *region);
void pmm_freelist_append_node(struct region *region, const signed int order, struct page *pg);
void pmm_freelist_remove_node(struct region *region, const struct buddy_freelist *node);