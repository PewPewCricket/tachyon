#pragma once

#define PAGE_SIZE 4096
#define PAGE_SHIFT 12
#define MAX_ORDER 16

#define ALIGN_UP(a) ((a + PAGE_SIZE - 1) / PAGE_SIZE)
#define ALIGN_DOWN(a) (a / PAGE_SIZE)

void init_mm();