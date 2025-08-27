#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <lib/string.h>
#include <limine/limine.h>
#include <limine/types.h>
#include <tachyon/bootreqs.h>
#include <drivers/fbcon.h>
#include <tachyon/phys_mem.h>
#include <tachyon/printk.h>

static volatile __limine_requests_start LIMINE_REQUESTS_START_MARKER;
static volatile __limine_requests_end LIMINE_REQUESTS_END_MARKER;
static volatile __limine_request LIMINE_BASE_REVISION(3);

void mk_mem_map();

static void hcf(void) {
    for (;;)
        asm ("hlt");
}

void boot(void) {
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    if (limine_framebuffer_request.response == NULL
    || limine_framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    const struct limine_framebuffer *framebuffer = limine_framebuffer_request.response->framebuffers[0];
    fbcon_setfb(framebuffer);
    mk_mem_map();

    hcf();
}

void mk_mem_map() {
    const struct limine_memmap_response *limine_memmap = limine_memmap_request.response;
    const uint64_t hhdm_offset = limine_hhdm_request.response->offset;
    bool mem_map_alloced = false;
    uint64_t mem_total = 0;

    for (uint64_t i = 0; i < limine_memmap->entry_count; i++) {
        const uint64_t length = limine_memmap->entries[i]->length;
        const uint64_t type = limine_memmap->entries[i]->type;
        if (type == LIMINE_MEMMAP_USABLE ||
            type == LIMINE_MEMMAP_ACPI_RECLAIMABLE ||
            type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE ||
            type == LIMINE_MEMMAP_ACPI_NVS ||
            type == LIMINE_MEMMAP_FRAMEBUFFER)
        {
            mem_total += length;
        }
    }

    printk(KERN_EMERG, "total memory: %llu bytes\n", mem_total);
    printk(KERN_DEBUG, "searching for usable region of %llu bytes\n", mem_total / 4096 * sizeof(struct page));

    uint64_t mem_map_size = 0;
    for (uint64_t i = 0; i < limine_memmap->entry_count; i++) {
        const uint64_t base = limine_memmap->entries[i]->base;
        const uint64_t length = limine_memmap->entries[i]->length;
        const uint64_t type = limine_memmap->entries[i]->type;
        mem_map_size = mem_total / 4096 * sizeof(struct page);

        if (length >= mem_map_size && type == LIMINE_MEMMAP_USABLE) {
            mem_map = (struct page*) (base + hhdm_offset);
            mem_map_alloced = true;
            break;
        }
    }

    if (!mem_map_alloced) {
        printk(KERN_EMERG, "mem_map allocation failed!\n");
        hcf();
    }

    const uint64_t mem_map_phys = (uint64_t)mem_map - hhdm_offset;
    const uint64_t mem_map_pages = (mem_map_size + 4095) / 4096;
    const uint64_t mem_map_first_pfn = mem_map_phys / 4096;
    const uint64_t mem_map_last_pfn  = mem_map_first_pfn + mem_map_pages - 1;
    uint64_t page_iter = 0;

    for (uint64_t i = 0; i < limine_memmap->entry_count; i++) {
        const uint64_t base = limine_memmap->entries[i]->base;
        const uint64_t base_page = base / 4096;
        const uint64_t length = limine_memmap->entries[i]->length;
        const uint64_t pages = (length + 4096 - 1) / 4096;
        const uint64_t type = limine_memmap->entries[i]->type;

        if (type == LIMINE_MEMMAP_USABLE) {
            printk(KERN_DEBUG, "found usable region: %p (%llu bytes)\n", base, length);
            for (uint64_t phys_page_iter = 0; phys_page_iter < pages && page_iter < mem_total/4096; phys_page_iter++, page_iter++) {
                mem_map[page_iter].pfn = base_page + phys_page_iter;
                mem_map[page_iter].refcount = 0;
                if (mem_map[page_iter].pfn >= mem_map_first_pfn &&
                    mem_map[page_iter].pfn <= mem_map_last_pfn)
                {
                    mem_map[page_iter].flags = PG_RESERVED;
                }
            }
        } else if (type == LIMINE_MEMMAP_ACPI_RECLAIMABLE || type ==LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE) {
            printk(KERN_DEBUG, "found reclaimable region: %p (%llu bytes)\n", base, length);
            for (uint64_t phys_page_iter = 0; phys_page_iter < pages && page_iter < mem_total/4096; phys_page_iter++, page_iter++) {
                mem_map[page_iter].pfn = base_page + phys_page_iter;
                mem_map[page_iter].refcount = 1;;
            }
        }else if (type == LIMINE_MEMMAP_ACPI_NVS || type == LIMINE_MEMMAP_FRAMEBUFFER) {
            printk(KERN_DEBUG, "found reserved region: %p (%llu bytes\n", base, length);
            for (uint64_t phys_page_iter = 0; phys_page_iter < pages && page_iter < mem_total/4096; phys_page_iter++, page_iter++) {
                mem_map[page_iter].pfn = base_page + phys_page_iter;
                mem_map[page_iter].flags = PG_RESERVED;
                mem_map[page_iter].refcount = 0;
            }
        } else {
            printk(KERN_DEBUG, "found nomap region: %p (%llu bytes)\n", base, length);
        }
    }

    printk(KERN_DEBUG, "ran mapping for %llu pages out of %llu\n", page_iter, mem_total / 4096);
    printk(KERN_DEBUG, "mm: created page map\n");
}