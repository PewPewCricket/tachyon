#include <limine/limine.h>
#include <limine/types.h>

static volatile __limine_request struct limine_stack_size_request limine_stack_size_request = {
	.id = LIMINE_STACK_SIZE_REQUEST,
	.revision = 0,
	.stack_size = 32768,
};

volatile __limine_request struct limine_memmap_request limine_memmap_request = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 3,
};

volatile __limine_request struct limine_bootloader_info_request limine_bootloader_info_request = {
	.id = LIMINE_BOOTLOADER_INFO_REQUEST,
	.revision = 0,
};

volatile __limine_request struct limine_executable_cmdline_request limine_executable_cmdline_request = {
	.id = LIMINE_EXECUTABLE_CMDLINE_REQUEST,
	.revision = 0,
};

volatile __limine_request struct limine_framebuffer_request limine_framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST,
	.revision = 0,
};

volatile __limine_request struct limine_hhdm_request limine_hhdm_request = {
	.id = LIMINE_HHDM_REQUEST,
	.revision = 0,
};

volatile __limine_request struct limine_paging_mode_request limine_paging_mode_request = {
	.id = LIMINE_PAGING_MODE_REQUEST,
	.revision = 0,
	.mode = LIMINE_PAGING_MODE_X86_64_4LVL,
	.max_mode = LIMINE_PAGING_MODE_X86_64_4LVL,
	.min_mode = LIMINE_PAGING_MODE_X86_64_4LVL,
};

volatile __limine_request struct limine_rsdp_request limine_rsdp_request = {
	.id = LIMINE_RSDP_REQUEST,
	.revision = 3,
};

volatile __limine_request struct limine_smbios_request limine_smbios_request = {
	.id = LIMINE_SMBIOS_REQUEST,
	.revision = 3,
};

volatile __limine_request struct limine_efi_system_table_request limine_efi_system_table_request = {
	.id = LIMINE_EFI_SYSTEM_TABLE_REQUEST,
	.revision = 3,
};

volatile __limine_request struct limine_efi_memmap_request limine_efi_memmap_request = {
	.id = LIMINE_EFI_MEMMAP_REQUEST,
	.revision = 0,
};

volatile __limine_request struct limine_date_at_boot_request limine_date_at_boot_request = {
	.id = LIMINE_DATE_AT_BOOT_REQUEST,
	.revision = 0,
};