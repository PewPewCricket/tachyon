#pragma once
#include <tachyon/pmm/pmem.h>

void pmm_buddy_split_block(struct region *region, struct buddy_freelist *node);
struct buddy_freelist *pmm_buddy_merge_block(struct region *region, struct buddy_freelist *node);