#include <stdint.h>
#include <multiboot2.h>

void *_kernel_ptr = nullptr;
uint32_t _kernel_size = 0;
struct multiboot_tag_framebuffer *_fb = nullptr;