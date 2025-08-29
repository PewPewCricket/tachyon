#pragma once

#define PAGE_SIZE 4096
#define PAGE_SHIFT 12

#define ALIGN_UP(a) (((a + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE)
#define ALIGN_DOWN(a) ((a / PAGE_SIZE) * PAGE_SIZE)
#define BUDDY_BLOCK_ORDER(n) (1ULL << (n))
#define BUDDY_BLOCK_REMAIN(x) ((x) & ~(1ULL << (63 - __builtin_clzll(x))))
#define BUDDY_BLOCK_PAGES(order) (1UL << (order))
#define BUDDY_BLOCK_BYTES(order) ((1UL << (order)) * PAGE_SIZE)

void mm_init();