#include <stdint.h>
#include <multiboot2.h>
#include <stddef.h>

void *_kernel_ptr;
uint32_t _kernel_size;
struct multiboot_tag_framebuffer *_fb;