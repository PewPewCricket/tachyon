#include <tachyon/mm/types.h>
#include <lib/align.h>
#include <limine/requests.h>
#include <limine/limine.h>
#include <tachyon/mm/buddy.h>
#include <tachyon/panic.h>
#include <tachyon/printk.h>

#include "stddef.h"

static struct limine_efi_memmap_response *efi_memmap;
static struct limine_memmap_response *limine_memmap;

uintptr_t hhdm_offset;

struct page *mem_map;
uintptr_t mem_map_count;
struct region *mem_regions;
uint64_t mem_regions_count;
struct free_area *free_area;

inline uintptr_t _get_total_usable_mem() {
	const uint64_t entry_count = limine_memmap->entry_count;
	uintptr_t total_mem = 0;

	// Get the total amount of usable memory in the system.
	for (uint64_t i = 0; i < entry_count; i++) {
		const struct limine_memmap_entry *entry = limine_memmap->entries[i];
		const uint64_t type   = entry->type;
		const uint64_t length = entry->length;

		if (type == LIMINE_MEMMAP_USABLE || type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE || type == LIMINE_MEMMAP_ACPI_RECLAIMABLE)
			total_mem += ALIGN_DOWN(length);
	}

	return total_mem;
}

inline uint64_t _get_usable_mem_regions() {
	const uint64_t entry_count = limine_memmap->entry_count;
	uint64_t region_counter = 0;

	// Get the total amount of usable memory in the system.
	for (uint64_t i = 0; i < entry_count; i++) {
		const struct limine_memmap_entry *entry = limine_memmap->entries[i];
		const uint64_t type   = entry->type;

		if (type == LIMINE_MEMMAP_USABLE || type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE || type == LIMINE_MEMMAP_ACPI_RECLAIMABLE)
			region_counter++;
	}

	return region_counter;
}

static void _alloc_static_mm_data() {
	const uintptr_t mem_total			= _get_total_usable_mem();
	const uintptr_t mem_map_size		= ADDR_TO_PFN(mem_total) * sizeof(struct page);
	const uintptr_t mem_regions_size	= _get_usable_mem_regions() * sizeof(struct region);
	const uintptr_t free_area_size		= (BUDDY_MAX_ORDER - 1) * sizeof(struct free_area);
	const uintptr_t mm_metadata_size	= mem_map_size + mem_regions_size + free_area_size;

	const uint64_t entry_count = limine_memmap->entry_count;
	for (uint64_t i = 0; i < entry_count; i++) {
		const struct limine_memmap_entry *entry = limine_memmap->entries[i];
		const uint64_t type   = entry->type;
		const uint64_t length = entry->length;
		const uint64_t base   = entry->base;

		if (length >= mm_metadata_size && type == LIMINE_MEMMAP_USABLE) {
			mem_map = (struct page *) (base + hhdm_offset);
			mem_regions = (struct region *) (base + mem_map_size + hhdm_offset);
			free_area = (struct free_area *) (base + mem_map_size + mem_regions_size + hhdm_offset);

			const uintptr_t mm_metadata_last = base + ALIGN_UP(mm_metadata_size);
			for (uintptr_t j = ADDR_TO_PFN(base); j < ADDR_TO_PFN(mm_metadata_last); j++) {
				mem_map[j].flags = PG_RESERVED;
				mem_map[j].refcount = 1;
				mem_map[j].lru.next = NULL;
				mem_map[j].lru.prev = NULL;
				mem_map[j].order = 0;

				mem_map_count = ADDR_TO_PFN(mem_total);
				mem_regions_count = _get_usable_mem_regions();
			}

			return;
		}
	}

	// We didn't find anywhere to put metadata.
	printk(KERN_EMERG, "failed to allocate memory manager usable physical memory metadata!\n");
	panic();
}

static void _init_mm_physical_metadata() {
	const uint64_t entry_count = limine_memmap->entry_count;
	uint64_t page_idx = 0;
	uint64_t region_idx = 0;

	for (uint64_t i = 0; i < entry_count; i++) {
		const struct limine_memmap_entry *entry = limine_memmap->entries[i];
		const uint64_t type   = entry->type;
		uint64_t length = entry->length;
		const uint64_t base   = entry->base;
		const uintptr_t base_pfn = ADDR_TO_PFN(base);
		const uintptr_t page_len = length >> PAGE_SHIFT;

		if (!(type == LIMINE_MEMMAP_USABLE || type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE || type == LIMINE_MEMMAP_ACPI_RECLAIMABLE))
			continue;

		// Setup page structs
		mem_regions[region_idx].map = &mem_map[page_idx];
		for (uintptr_t j = 0; j < page_len; j++) {
			mem_map[page_idx].pfn = base_pfn + j;
			page_idx++;
		}

		mem_regions[region_idx].length = page_len;

		region_idx++;
	}
}

void mm_init() {
	// set up needed limine structs
	efi_memmap = limine_efi_memmap_request.response;
	limine_memmap = limine_memmap_request.response;
	hhdm_offset = limine_hhdm_request.response->offset;

	// Split up for clarity (and breakpoints!)
	_alloc_static_mm_data();
	_init_mm_physical_metadata();

	printk(KERN_DEBUG, "%llu MiB available (%llu bytes).\n", _get_total_usable_mem() / (1024 * 1024), _get_total_usable_mem());

	for (uint64_t i = 0; i < mem_regions_count; i++) {
		printk(KERN_DEBUG, "region %llu:\tbase pfn: %llu\t\tspan: %llu pages\n\t\tbase: %p\t\tto: %p\t\tlength: %llu\n",
			i, mem_regions[i].map->pfn, mem_regions[i].length, PFN_TO_ADDR(mem_regions[i].map->pfn), PFN_TO_ADDR(mem_regions[i].map->pfn) + PFN_TO_ADDR(mem_regions[i].length), mem_regions[i].length * PAGE_SIZE);
	}
}
