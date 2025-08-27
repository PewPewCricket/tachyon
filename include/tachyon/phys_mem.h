#pragma once
#include <stdint.h>

enum page_flags {
	PG_NONE			= 0,
	PG_RESERVED		= 1 << 0,
	PG_DIRTY		= 1 << 1,
	PG_LOCKED		= 1 << 2,
};

enum zone_type {
	ZONE_DMA,
	ZONE_DMA32,
	ZONE_NORMAL,
};

struct page {
	enum page_flags flags;
	uint64_t pfn;
	uint16_t refcount;
};

struct zone {
	enum zone_type type;
	uint64_t zone_start_pfn;
	uint64_t spanned_pages;
	uint64_t present_pages;
};

struct page *mem_map;