#include <limine/limine.h>
#include <limine/requests.h>
#include <tachyon/printk.h>
#include <tachyon/panic.h>
#include <tachyon/memory.h>
#include <tachyon/pmm/pmem.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

struct page *pmm_map;
struct region *pmm_regions;

static void pmm_init_region_freelist(struct region *region) {
    uint64_t page_idx = 0;
    uint64_t pages_remaining = region->length;

    while (pages_remaining > 0) {
        // largest block that fits
        int order = 63 - __builtin_clzll(pages_remaining);
        // page_idx must be divisible by block size
        while (page_idx & ((1ULL << order) - 1))
            order--;
        const uint64_t block_size = 1ULL << order;

        pmm_freelist_append_node(region, order, &region->map[page_idx]);

        // move to next block
        page_idx += block_size;
        pages_remaining -= block_size;
    }
}


static void pmm_init_data(struct page *pmm_map, struct region *pmm_regions, const uint64_t mm_data_size, const uint64_t total_pages) {
    const struct limine_memmap_response *limine_memmap = limine_memmap_request.response;
    const uintptr_t pmm_map_phys        = (uint64_t) pmm_map - mem_hhdm_offset;
    const uint64_t mm_data_first_pfn    = ALIGN_DOWN(pmm_map_phys) / PAGE_SIZE;
    const uint64_t mm_data_last_pfn     = ALIGN_UP(pmm_map_phys + mm_data_size) / PAGE_SIZE;
    uint64_t page_iter = 0;
    uint64_t region_idx = 0;

    for (uint64_t i = 0; i < limine_memmap->entry_count; i++) {
        const uintptr_t base    = limine_memmap->entries[i]->base;
        const uint64_t length   = limine_memmap->entries[i]->length;
        const uint64_t pages    = ALIGN_UP(length) / PAGE_SIZE;
        const uint64_t type     = limine_memmap->entries[i]->type;

        // Logging
        if (type == LIMINE_MEMMAP_USABLE)
            printk(KERN_DEBUG, "mm: found usable region: %p (pfn %llu) (%llu bytes)\n", base, base / 4096, length);
        else if (type == LIMINE_MEMMAP_ACPI_RECLAIMABLE || type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE)
            printk(KERN_DEBUG, "mm: found reclaimable region: %p (pfn %llu) (%llu bytes)\n", base, base / 4096, length);

        // only map memory that we have a reason to know about
        if (type == LIMINE_MEMMAP_USABLE ||
            type == LIMINE_MEMMAP_ACPI_RECLAIMABLE ||
            type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE)
        {
            pmm_regions[region_idx].freelist_head = NULL;
            pmm_regions[region_idx].freelist_tail = NULL;
            pmm_regions[region_idx].map = pmm_map + page_iter;
            pmm_regions[region_idx].length = pages;

            // fill the mem map entries of current region
            for (uint64_t j = 0; j < pages && page_iter < total_pages; j++, page_iter++) {
                pmm_map[page_iter].pfn = base / PAGE_SIZE + j;
                pmm_map[page_iter].flags = 0;
                pmm_map[page_iter].refcount = 0;

                if (type == LIMINE_MEMMAP_USABLE) {
                    // This might reserve 1 extra page, but im too scared to change this so it's staying as it is!
                    if (pmm_map[page_iter].pfn >= mm_data_first_pfn && pmm_map[page_iter].pfn <= mm_data_last_pfn) {
                        pmm_map[page_iter].flags = PG_RESERVED;

                        // It's easier to pretend the pages just don't exist.
                        pmm_regions[region_idx].map++;
                        pmm_regions[region_idx].length--;
                    }

                } else { // Reclaimable
                    pmm_map[page_iter].flags = PG_RESERVED;
                }
            }

            // if we build a freelist for memory we don't want to overwrite bad thing happen!
            if (type == LIMINE_MEMMAP_USABLE)
                pmm_init_region_freelist(&pmm_regions[region_idx]);

            region_idx++;
        } else {
            printk(KERN_DEBUG, "mm: found nomap region: %p (pfn %llu) (%llu bytes)\n", base, base / 4096, length);
        }
    }

    printk(KERN_DEBUG, "mm: ran mapping for %llu pages out of %llu\n", page_iter, total_pages);
    printk(KERN_DEBUG, "mm: created memory map\n");
}

void pmm_init() {
    const struct limine_memmap_response *limine_memmap = limine_memmap_request.response;
    uint64_t region_count = 0;
    const uint64_t total_pages = pmm_get_total_pages(&region_count);
    bool mm_data_allocated = false;
    printk(KERN_EMERG, "mm: total memory: %llu pages (%llu bytes)\n", total_pages, total_pages * PAGE_SIZE);

    const uint64_t pmm_map_size     = total_pages * sizeof(struct page);
    const uint64_t pmm_regions_size = region_count * sizeof(struct region);
    const uint64_t mm_data_size     = pmm_map_size + pmm_regions_size;

    // Allocate space for memory metadata (who needs a boot-time allocator!)
    printk(KERN_DEBUG, "mm: searching for usable region of %llu pages (%llu bytes)\n", ALIGN_UP(mm_data_size) / PAGE_SIZE, mm_data_size);

    for (uint64_t i = 0; i < limine_memmap->entry_count; i++) {
        const uintptr_t base    = limine_memmap->entries[i]->base;
        const uintptr_t length  = limine_memmap->entries[i]->length;
        const uintptr_t type    = limine_memmap->entries[i]->type;

        if (length >= mm_data_size && type == LIMINE_MEMMAP_USABLE) {
            pmm_map     = (struct page*) (base + mem_hhdm_offset);
            pmm_regions = (struct region*) (pmm_map + total_pages);
            mm_data_allocated = true;
            break;
        }
    }

    // We done fucked up!
    if (!mm_data_allocated) {
        printk(KERN_EMERG, "mm physical map allocation failed!\n");
        panic();
    }

    pmm_init_data(pmm_map, pmm_regions, mm_data_size, total_pages);

    // Big ass debug print for sanity checks
    for (uint64_t i = 0; i < region_count; i++) {
        printk(KERN_DEBUG, "mm: region %llu: %p (pfn %llu) - %p (page %llu) for %llu pages ", i,
            pmm_regions[i].map[0].pfn * PAGE_SIZE,

            pmm_regions[i].map[0].pfn,
            pmm_regions[i].map[pmm_regions[i].length - 1].pfn * PAGE_SIZE + 4095,
            pmm_regions[i].map[pmm_regions[i].length - 1].pfn,
            pmm_regions[i].length
        );

        if (pmm_regions[i].map->flags == PG_RESERVED)
            printk(KERN_DEBUG, "(reclaimable)\n");
        else
            printk(KERN_DEBUG, "(memory)\n");

        pmm_print_freelist(&pmm_regions[i]);
    }
}