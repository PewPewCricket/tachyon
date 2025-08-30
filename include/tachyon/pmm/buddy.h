#pragma once
#include <tachyon/pmm/pmem.h>
#include <stdbool.h>

bool pmm_buddy_split_block(struct region *region, const struct buddy_freelist *node);
struct buddy_freelist *pmm_buddy_merge_block(struct region *region, struct buddy_freelist *node);