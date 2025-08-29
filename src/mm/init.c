#define PMEM_H_NO_MEM_MAP 1
#include <limine/limine.h>
#include <tachyon/bootreqs.h>
#include <tachyon/printk.h>
#include <tachyon/phys_mem.h>
#include <tachyon/panic.h>
#include <tachyon/memory.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

struct page *mem_map;
struct region *mem_regions;
struct zone *mem_zones;

static uint64_t _get_total_pages(uint64_t *region_count) {
    const struct limine_memmap_response *limine_memmap = limine_memmap_request.response;
    uint64_t mem_total = 0;
    uint64_t rc = 0;

    for (uint64_t i = 0; i < limine_memmap->entry_count; i++) {
        const uint64_t length = limine_memmap->entries[i]->length;
        const uint64_t type = limine_memmap->entries[i]->type;
        if (type == LIMINE_MEMMAP_USABLE ||
            type == LIMINE_MEMMAP_ACPI_RECLAIMABLE ||
            type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE ||
            type == LIMINE_MEMMAP_ACPI_NVS ||
            type == LIMINE_MEMMAP_FRAMEBUFFER)
        {
            rc++;
            mem_total += length - (length % PAGE_SIZE);
        }
    }

    *region_count = rc;
    return mem_total / PAGE_SIZE;
}

void mm_print_region_freelist(const struct region *region, const uint64_t hhdm_offset) {
    const struct buddy_freelist *node = region->freelist_head;
    if (node)
        printk(KERN_DEBUG, "mm: freelist for region %p for %llu pages:\n", region->map->pfn * PAGE_SIZE, region->length);

    while (node) {
        const uintptr_t pfn = node->page->pfn;
        const int order         = node->page->order;
        const char *prev_flag   = node->prev ? "P" : ".";
        const char *next_flag   = node->next ? "N" : ".";
        printk(KERN_DEBUG, "  [%s%s] PFN: %lu, order: %d\n",
               prev_flag, next_flag, pfn, order);
        node = node->next;
    }
}

static void mm_add_region_freelist(struct region *region, const signed int order, struct page *pg, const uint64_t hhdm_offset) {
    const uintptr_t node_addr = (pg->pfn << PAGE_SHIFT) + hhdm_offset;
    struct buddy_freelist *node = (struct buddy_freelist *)node_addr;

    node->page = pg;
    node->next = NULL;
    node->prev = region->freelist_tail;

    // first page in block is set ot its order, pages inside the block are set to -1
    pg->order = order;
    const size_t idx = pg - region->map;
    for (size_t i = 1; i < (1UL << order); i++) {
        region->map[idx + i].order = -1;
    }

    if (region->freelist_tail) {
        region->freelist_tail->next = node;
    } else {
        region->freelist_head = node;
    }
    region->freelist_tail = node;
}

// implement later (ugghh)
static void _mm_init_region_freelist(struct region *region, const uintptr_t hhdm_offset) {
    uint64_t pg_iter = 0;
    uint64_t reserved_pages = 0;
    bool is_parsing_block = false;
    while (pg_iter < region->length) {
        struct page *pg = &region->map[pg_iter];

        if (pg->flags & PG_RESERVED && !is_parsing_block) {
            reserved_pages++;
        }
        pg_iter++;
    }
    if (reserved_pages)
        printk(KERN_DEBUG, "%llu reserved pages\n", reserved_pages);
}

static void _mm_init_data(struct page *mem_map, struct region *mem_regions, struct zone *mem_zones,
                             const uint64_t mm_data_size, const uint64_t total_pages)
{
    const struct limine_memmap_response *limine_memmap = limine_memmap_request.response;
    const uintptr_t hhdm_offset         = limine_hhdm_request.response->offset;
    const uintptr_t mem_map_phys        = (uint64_t) mem_map - hhdm_offset;
    const uint64_t mm_data_first_pfn    = ALIGN_DOWN_TO_PFN(mem_map_phys);
    const uint64_t mm_data_last_pfn     = ALIGN_UP_TO_PFN(mem_map_phys + mm_data_size);
    uint64_t page_iter = 0;
    uint64_t region_idx = 0;

    for (uint64_t i = 0; i < limine_memmap->entry_count; i++) {
        const uintptr_t base    = limine_memmap->entries[i]->base;
        const uint64_t length   = limine_memmap->entries[i]->length;
        const uint64_t pages    = ALIGN_UP_TO_PFN(length);
        const uint64_t type     = limine_memmap->entries[i]->type;
        struct buddy_freelist *last_node = NULL;

        // Logging
        if (type == LIMINE_MEMMAP_USABLE) {
            printk(KERN_DEBUG, "mm: found usable region: %p (pfn %llu) (%llu bytes)\n", base, base / 4096, length);
        } else if (type == LIMINE_MEMMAP_ACPI_RECLAIMABLE || type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE) {
            printk(KERN_DEBUG, "mm: found reclaimable region: %p (pfn %llu) (%llu bytes)\n", base, base / 4096, length);
        } else if (type == LIMINE_MEMMAP_ACPI_NVS || type == LIMINE_MEMMAP_FRAMEBUFFER) {
            printk(KERN_DEBUG, "mm: found device region: %p (pfn %llu) (%llu bytes)\n", base, base / 4096, length);
        }

        // only map memory that we have a reason to know about
        if (type == LIMINE_MEMMAP_USABLE ||
            type == LIMINE_MEMMAP_ACPI_RECLAIMABLE ||
            type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE ||
            type == LIMINE_MEMMAP_ACPI_NVS ||
            type == LIMINE_MEMMAP_FRAMEBUFFER)
        {
            mem_regions[region_idx].map = mem_map + page_iter;
            mem_regions[region_idx].length = pages;
            mem_regions[region_idx].type = (type == LIMINE_MEMMAP_USABLE || type == LIMINE_MEMMAP_ACPI_RECLAIMABLE ||
                type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE) ? MEM_NORMAL : MEM_DEVICE;

            // fill the mem map entries of current region
            for (uint64_t j = 0; j < pages && page_iter < total_pages; j++, page_iter++) {
                mem_map[page_iter].pfn = base / PAGE_SIZE + j;
                mem_map[page_iter].flags = 0;
                mem_map[page_iter].refcount = 0;

                if (type == LIMINE_MEMMAP_USABLE) {
                    // This might reserve 1 extra page, but im too scared to change this so it's staying as it is!
                    if (mem_map[page_iter].pfn >= mm_data_first_pfn && mem_map[page_iter].pfn <= mm_data_last_pfn)
                        mem_map[page_iter].flags = PG_RESERVED;

                } else if (type == LIMINE_MEMMAP_ACPI_RECLAIMABLE || type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE) {
                    mem_map[page_iter].refcount = 1;

                } else { // device / framebuffer / NVS
                    mem_map[page_iter].flags = PG_RESERVED;
                }
            }

            // if we build a freelist for memory we don't want to overwrite bad thing happen!
            if (type == LIMINE_MEMMAP_USABLE)  {
                _mm_init_region_freelist(&mem_regions[region_idx], hhdm_offset);
            }

            region_idx++;
        } else printk(KERN_DEBUG, "mm: found nomap region: %p (pfn %llu) (%llu bytes)\n", base, base / 4096, length);
    }

    printk(KERN_DEBUG, "mm: ran mapping for %llu pages out of %llu\n", page_iter, total_pages);
    printk(KERN_DEBUG, "mm: created memory map\n");
}

void init_mm() {
    const struct limine_memmap_response *limine_memmap = limine_memmap_request.response;
    const uint64_t hhdm_offset = limine_hhdm_request.response->offset;
    uint64_t region_count = 0;
    const uint64_t total_pages = _get_total_pages(&region_count);
    bool mm_data_allocated = false;
    printk(KERN_EMERG, "mm: total memory: %llu pages (%llu bytes)\n", total_pages, total_pages * PAGE_SIZE);

    const uint64_t mem_map_size     = total_pages * sizeof(struct page);
    const uint64_t mem_regions_size = region_count * sizeof(struct region);
    const uint64_t mem_zones_size   = 4 * sizeof(struct zone); // 4 possible types of zones
    const uint64_t mm_data_size     = mem_map_size + mem_regions_size + mem_zones_size;

    // Allocate space for memory metadata (who needs a boot-time allocator!)
    printk(KERN_DEBUG, "mm: searching for usable region of %llu pages (%llu bytes)\n", ALIGN_UP_TO_PFN(mm_data_size), mm_data_size);

    for (uint64_t i = 0; i < limine_memmap->entry_count; i++) {
        const uintptr_t base    = limine_memmap->entries[i]->base;
        const uintptr_t length  = limine_memmap->entries[i]->length;
        const uintptr_t type    = limine_memmap->entries[i]->type;

        if (length >= mm_data_size && type == LIMINE_MEMMAP_USABLE) {
            mem_map     = (struct page*) (base + hhdm_offset);
            mem_regions = (struct region*) (mem_map + total_pages);
            mem_zones   = (struct zone*) (mem_regions + region_count);
            mm_data_allocated = true;
            break;
        }
    }

    // We done fucked up!
    if (!mm_data_allocated) {
        printk(KERN_EMERG, "mm physical map allocation failed!\n");
        panic();
    }

    _mm_init_data(mem_map, mem_regions, mem_zones, mm_data_size, total_pages);

    // Big ass debug print for sanity checks
    for (uint64_t i = 0l; i < region_count; i++) {
        printk(KERN_DEBUG, "mm: region %u: %p (pfn %llu) - %p (page %llu) for %llu pages ", i,
            mem_regions[i].map[0].pfn * PAGE_SIZE,
            mem_regions[i].map[0].pfn,
            mem_regions[i].map[mem_regions[i].length - 1].pfn * PAGE_SIZE + 4095,
            mem_regions[i].map[mem_regions[i].length - 1].pfn,
            mem_regions[i].length
        );

        if (mem_regions[i].type == MEM_NORMAL)
            printk(KERN_DEBUG, "(memory)\n");
        else if (mem_regions[i].type == MEM_DEVICE)
            printk(KERN_DEBUG, "(device)\n");
        else
            printk(KERN_DEBUG, "(unknown)\n");

        mm_print_region_freelist(&mem_regions[i], hhdm_offset);
    }
}