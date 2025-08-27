#include <limine/limine.h>
#include <tachyon/bootreqs.h>
#include <tachyon/printk.h>
#define PMEM_H_NO_MEM_MAP 1
#include <tachyon/phys_mem.h>
#include <tachyon/panic.h>
#include <stdint.h>
#include <stdbool.h>

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
            mem_total += length - (length % 4096);
        }
    }

    *region_count = rc;
    return mem_total / 4096;
}

static void _mm_init_data(struct page *mem_map, struct region *mem_regions, struct zone *mem_zones,
                             const uint64_t mm_data_size, const uint64_t total_pages)
{
    const struct limine_memmap_response *limine_memmap = limine_memmap_request.response;
    const uintptr_t hhdm_offset = limine_hhdm_request.response->offset;
    const uintptr_t mem_map_phys = (uint64_t) mem_map - hhdm_offset;
    const uint64_t mm_data_first_pfn = (mem_map_phys + 4096 - 1) / 4096;
    const uint64_t mm_data_last_pfn  = (mem_map_phys + mm_data_size + 4096 - 1) / 4096;
    uint64_t page_iter = 0;
    uint64_t region_idx = 0;

    for (uint64_t i = 0; i < limine_memmap->entry_count; i++) {
        const uintptr_t base = limine_memmap->entries[i]->base;
        const uint64_t length = limine_memmap->entries[i]->length;
        const uint64_t pages = (length + 4096 - 1) / 4096;
        const uint64_t type = limine_memmap->entries[i]->type;

        // Logging
        if (type == LIMINE_MEMMAP_USABLE) {
            printk(KERN_DEBUG, "mm: found usable region: %p (%llu bytes)\n", base, length);
        } else if (type == LIMINE_MEMMAP_ACPI_RECLAIMABLE || type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE) {
            printk(KERN_DEBUG, "mm: found reclaimable region: %p (%llu bytes)\n", base, length);
        } else if (type == LIMINE_MEMMAP_ACPI_NVS || type == LIMINE_MEMMAP_FRAMEBUFFER) {
            printk(KERN_DEBUG, "mm: found device region: %p (%llu bytes)\n", base, length);
        }

        if (type == LIMINE_MEMMAP_USABLE ||
            type == LIMINE_MEMMAP_ACPI_RECLAIMABLE ||
            type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE ||
            type == LIMINE_MEMMAP_ACPI_NVS ||
            type == LIMINE_MEMMAP_FRAMEBUFFER)
        {
            mem_regions[region_idx].map = mem_map + page_iter;
            mem_regions[region_idx].length = pages;
            mem_regions[region_idx].type = (type == LIMINE_MEMMAP_USABLE || type == LIMINE_MEMMAP_ACPI_RECLAIMABLE || type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE) ? MEM_NORMAL : MEM_DEVICE;

            // fill the mem map entries of current region
            for (uint64_t j = 0; j < pages && page_iter < total_pages; j++, page_iter++) {
                mem_map[page_iter].pfn = base / 4096 + j;

                if (type == LIMINE_MEMMAP_USABLE) {
                    mem_map[page_iter].refcount = 0;
                    if (mem_map[page_iter].pfn >= mm_data_first_pfn && mem_map[page_iter].pfn < mm_data_last_pfn)
                        mem_map[page_iter].flags = PG_RESERVED;
                } else if (type == LIMINE_MEMMAP_ACPI_RECLAIMABLE || type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE) {
                    mem_map[page_iter].refcount = 1;
                } else { // device / framebuffer / NVS
                    mem_map[page_iter].refcount = 0;
                    mem_map[page_iter].flags = PG_RESERVED;
                }
            }

            region_idx++;
        } else printk(KERN_DEBUG, "mm: found nomap region: %p (%llu bytes)\n", base, length);
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
    printk(KERN_EMERG, "mm: total memory: %llu pages (%llu bytes)\n", total_pages, total_pages * 4096);

    const uint64_t mem_map_size = total_pages * sizeof(struct page);
    const uint64_t mem_regions_size = region_count * sizeof(struct region);
    const uint64_t mem_zones_size   = 4 * sizeof(struct zone); // 4 possible types of zones
    const uint64_t mm_data_size = mem_map_size + mem_regions_size + mem_zones_size;

    // Allocate space for memory metadata (who needs a boottime allocator!)
    printk(KERN_DEBUG, "mm: searching for usable region of %llu bytes\n", total_pages * sizeof(struct page));

    for (uint64_t i = 0; i < limine_memmap->entry_count; i++) {
        const uintptr_t base = limine_memmap->entries[i]->base;
        const uintptr_t length = limine_memmap->entries[i]->length;
        const uintptr_t type = limine_memmap->entries[i]->type;

        if (length >= mm_data_size && type == LIMINE_MEMMAP_USABLE) {
            mem_map = (struct page*) (base + hhdm_offset);
            mem_regions = (struct region*) (mem_map + total_pages);
            mem_zones   = (struct zone*) (mem_regions + region_count);
            mm_data_allocated = true;
            break;
        }
    }

    if (!mm_data_allocated) {
        printk(KERN_EMERG, "mm physical map allocation failed!\n");
        panic();
    }

    _mm_init_data(mem_map, mem_regions, mem_zones, mm_data_size, total_pages);

    for (uint64_t i = 0l; i < region_count; i++) {
        printk(KERN_DEBUG, "mm: region %u: %p - %p for %llu bytes ", i, mem_regions[i].map[0].pfn * 4096, mem_regions[i].map[mem_regions[i].length - 1].pfn * 4096 + 4095, mem_regions[i].length * 4096);
        if (mem_regions[i].type == MEM_NORMAL) printk(KERN_DEBUG, "(memory)\n");
        else if (mem_regions[i].type == MEM_DEVICE) printk(KERN_DEBUG, "(device)\n");
        else printk(KERN_DEBUG, "(unknown)\n");
    }
}