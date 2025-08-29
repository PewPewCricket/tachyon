#pragma once

#define PAGE_SIZE 4096
#define PAGE_SHIFT 12

#define ALIGN_UP_TO_PFN(a) ((a + PAGE_SIZE - 1) / PAGE_SIZE)
#define ALIGN_DOWN_TO_PFN(a) (a / PAGE_SIZE)
#define POW2_INDEX(n) (1ULL << (n))
#define POW2_REMAIN(x) ((x) & ~(1ULL << (63 - __builtin_clzll(x))))

void init_mm();