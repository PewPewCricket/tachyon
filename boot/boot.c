#include <stdint.h>
#include <tachyon/multiboot2.h>
#include <tachyon/debug/boot_debug.h>

void boot(void *mb2_data) {
#ifdef KDEBUG
	// Serial Port Logging
	serial_init();
	boot_log_serial("kernel running in debug mode.\n");
#endif

	// Parse multiboot2 tag structs
	uint8_t *tag_ptr = mb2_data + 8; // Skip total_size and reserved
	const uint32_t total_size = *(uint32_t*)tag_ptr;

	while (tag_ptr < ((uint8_t*)mb2_data) + total_size) {
		const uint32_t tag_type = *(uint32_t*)tag_ptr;
		const uint32_t tag_size = *(uint32_t*)(tag_ptr + 4);
		switch (tag_type) {
			case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
				auto tag_fb = (struct multiboot_tag_framebuffer*)tag_ptr;
				switch (tag_fb->common.framebuffer_type) {
					case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
						break;
					case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
						break;
					case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
						break;
					default:;
				}
				break;
			case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
				auto tag_loader_name = (struct multiboot_tag_string*)tag_ptr;
				break;
			case MULTIBOOT_TAG_TYPE_MMAP:
				auto tag_mmap = (struct multiboot_tag_mmap*)tag_ptr;
				break;
			default:;
		}

		// Tags are aligned to 8 bytes
		tag_ptr += (tag_size + 7) & ~7;
	}
}
