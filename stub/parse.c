#include <stdint.h>
#include <multiboot2.h>
#include "scrio.h"
#include "util.h"
#include "globals.h"

void mb2_parse_tags(void *tags) {
	void *tags_begin = tags;
	_kernel_ptr = NULL;
	bool running = true;
	tags += 8; // Skip tags list header

	// Get framebuffer first so all tags can be debugged.
	while (running) {
		const uint32_t tag_type = *(uint32_t*)tags;
		const uint32_t tag_size = *(uint32_t*)(tags + 4);
		switch (tag_type) {
			case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
				_fb = (struct multiboot_tag_framebuffer *)tags;
				fbprintf("Got Framebuffer!\n");
				break;
			case MULTIBOOT_TAG_TYPE_END:
				running = false;
				break;
			default:
				break;
		}
		// Align to 8-byte boundary
		tags += (tag_size + 7) & ~7;
	}

	fbprintf("Getting boot tags:\n");
	tags = tags_begin;
	tags += 8;
	running = true;
	while (running) {
		const uint32_t tag_type = *(uint32_t*)tags;
		const uint32_t tag_size = *(uint32_t*)(tags + 4);

		switch (tag_type) {
			case MULTIBOOT_TAG_TYPE_MODULE:
				auto const tag_mod = (struct multiboot_tag_module *)tags;
				fbprintf("\tmodule2: %s [%x - %x]\n", tag_mod->cmdline, tag_mod->mod_start, tag_mod->mod_end);
				if (!strcmp(tag_mod->cmdline, "kernel64")) {
					_kernel_ptr = (void*) tag_mod->mod_start;
					_kernel_size = tag_mod->mod_end - tag_mod->mod_start;
				}
				break;
			case MULTIBOOT_TAG_TYPE_END:
				running = false;
				break;
			default:
				break;
		}

		// Align to 8-byte boundary
		tags += (tag_size + 7) & ~7;
	}

	if (_kernel_ptr == NULL) {
		fbprintf("ERROR: could not locate module2 \"kernel64\".\n");
		die();
	}
}