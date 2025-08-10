#include <stdint.h>
#include <tachyon/multiboot2.h>
#include <tachyon/debug/boot_debug.h>

void boot(void *mb2_data) {
#ifdef KDEBUG
	_boot_debug_serial_init();
#endif
	char buf[256];
	BOOT_LOG_SERIAL("Tachyon early boot serial logger.\n")



	BOOT_LOG_SERIAL("Multiboot2 data is at 0x")
	BOOT_LOG_SERIAL(_boot_debug_utoa((unsigned int)mb2_data, buf, 16));
	BOOT_LOG_SERIAL("\n")

	// Parse multiboot2 tag structs
	uint8_t *tag_ptr = mb2_data + 8; // Skip total_size and reserved

	bool tags_end_reached = false;
	while (!tags_end_reached) {
		const uint32_t tag_type = *(uint32_t*)tag_ptr;
		const uint32_t tag_size = *(uint32_t*)(tag_ptr + 4);
		switch (tag_type) {
			case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
				const auto tag_fb = (struct multiboot_tag_framebuffer*)tag_ptr;
				switch (tag_fb->common.framebuffer_type) {
					case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
						BOOT_LOG_SERIAL("Found RGB framebuffer tag.\n")
						break;
					case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED:
						BOOT_LOG_SERIAL("Found indexed framebuffer tag.\n")
						break;
					case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
						BOOT_LOG_SERIAL("Found EGA framebuffer tag.\n")
						break;
					default:;
				}
				break;
			case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
				BOOT_LOG_SERIAL("Found bootloader name tag.\n")
				auto tag_loader_name = (struct multiboot_tag_string*)tag_ptr;
				break;
			case MULTIBOOT_TAG_TYPE_MMAP:
				BOOT_LOG_SERIAL("Found memory map tag.\n")
				auto tag_mmap = (struct multiboot_tag_mmap*)tag_ptr;
				break;
			case MULTIBOOT_TAG_TYPE_END:
				BOOT_LOG_SERIAL("Found end tag, stopping.\n")
				tags_end_reached = true;
				break;
			default:
				BOOT_LOG_SERIAL("Found unknown tag: ")
				BOOT_LOG_SERIAL(_boot_debug_utoa(tag_type, buf, 10))
				BOOT_LOG_SERIAL("\n")
		}

		// Tags are aligned to 8 bytes
		tag_ptr += (tag_size + 7) & ~7;
	}
	BOOT_LOG_SERIAL("Multiboot2 data parsing finished.\n")
}