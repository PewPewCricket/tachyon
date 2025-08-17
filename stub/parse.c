#include <stdint.h>
#include <multiboot2.h>
#include "scrio.h"
#include "util.h"
#include "globals.h"

void mb2_parse_tags(const void *tags) {
	bool running = true;
	tags += 8; // Skip tags list header
	while (running) {
		const uint32_t tag_type = *(uint32_t*)tags;
		const uint32_t tag_size = *(uint32_t*)(tags + 4);

		switch (tag_type) {
			case MULTIBOOT_TAG_TYPE_MODULE:
				auto const tag_mod = (struct multiboot_tag_module *)tags;
				if (!strcmp(tag_mod->cmdline, "kernel64")) {
					_kernel_ptr = (void*) tag_mod->mod_start;
					_kernel_size = tag_mod->mod_end - tag_mod->mod_start;
				}
				break;
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

	if (_kernel_ptr == nullptr) {
		fbprintf("ERROR: could not locate module2 \"kernel64\".\n");
		_die();
	}

	fbprintf("fbprintf test:\n\t%s\t%d\t%u\t%x\n", "test_string", -25, 500, 0xdeadbeef);
}