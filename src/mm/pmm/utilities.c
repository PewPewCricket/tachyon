#include <limine/limine.h>
#include <limine/requests.h>
#include <tachyon/printk.h>
#include <tachyon/pmm/pmem.h>
#include <tachyon/memory.h>

uint64_t pmm_get_total_pages(uint64_t *region_count) {
    const struct limine_memmap_response *limine_memmap = limine_memmap_request.response;
    uint64_t mem_total = 0;
    uint64_t rc = 0;

    for (uint64_t i = 0; i < limine_memmap->entry_count; i++) {
        const uint64_t length = limine_memmap->entries[i]->length;
        const uint64_t type = limine_memmap->entries[i]->type;
        if (type == LIMINE_MEMMAP_USABLE ||
            type == LIMINE_MEMMAP_ACPI_RECLAIMABLE ||
            type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE)
        {
            rc++;

            mem_total += length - (length % PAGE_SIZE);
        }
    }

    *region_count = rc;
    return mem_total / PAGE_SIZE;
}

void pmm_print_freelist(const struct region *region) {
    const struct buddy_freelist *node = region->freelist_head;
    if (!node) return;

    printk(KERN_DEBUG, "mm: freelist for region %p for %llu pages:\n", region->map->pfn * PAGE_SIZE, region->length);

    while (node) {
        const uintptr_t pfn = node->page->pfn;
        const signed int order  = node->page->order;
        const char *prev_flag   = node->prev ? "P" : ".";
        const char *next_flag   = node->next ? "N" : ".";
        printk(KERN_DEBUG, "  [%s%s] PFN: %llu, order: %d\n", prev_flag, next_flag, pfn, order);
        node = node->next;
    }
}

void pmm_freelist_append_node(struct region *region, const signed int order, struct page *pg) {
    const uintptr_t node_addr = pg->pfn * PAGE_SIZE + mem_hhdm_offset;
    struct buddy_freelist *node = (struct buddy_freelist*) node_addr;

    node->page = pg;
    node->next = NULL;
    node->prev = region->freelist_tail;

    // first page in block is set ot its order, pages inside the block are set to -1
    pg->order = order;
    const size_t idx = pg - region->map;
    for (size_t i = 1; i < (1UL << order); i++) {
        region->map[idx + i].order = -1;
    }

    if (region->freelist_tail)
        region->freelist_tail->next = node;
    else
        region->freelist_head = node;

    region->freelist_tail = node;
}

void pmm_freelist_remove_node(struct region *region, const struct buddy_freelist *node) {
    struct buddy_freelist *prev = node->prev;
    struct buddy_freelist *next = node->next;
    struct page *pg = node->page;
    const signed int order = pg->order;

    if (next)
        next->prev = prev;
    else
        region->freelist_tail = prev;

    if (prev)
        prev->next = next;
    else
        region->freelist_head = next;

    const uint64_t page_count = BUDDY_BLOCK_PAGES(order);
    for (uint64_t i = 0; i < page_count; i++) {
        pg[i].order = -1;
    }
}