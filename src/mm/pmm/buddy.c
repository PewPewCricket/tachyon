#include <tachyon/pmm/pmem.h>
#include <tachyon/pmm/buddy.h>
#include <lib/math.h>
#include <stdbool.h>

bool pmm_buddy_split_block(struct region *region, const struct buddy_freelist *node) {
	const signed int old_order = node->page->order;
	const signed int new_order = node->page->order - 1;
	if (old_order <= 0)
		return false;

	const uintptr_t block_pfn = pow(2, old_order) + node->page->pfn;
	const uintptr_t block_page_offset = block_pfn - region->map->pfn;
	struct page *new_block_page = &region->map[block_page_offset];

	node->page->order = new_order;
	pmm_freelist_append_node(region, new_order, new_block_page);
	return true;
}

struct buddy_freelist *pmm_buddy_merge_block(struct region *region, struct buddy_freelist *node) {

}