#ifndef GLOBALS_H
#define GLOBALS_H
#include <stdint.h>
#include <multiboot2.h>

extern void *_kernel_ptr;
extern uint32_t _kernel_size;
extern struct multiboot_tag_framebuffer *_fb;

#endif //GLOBALS_H
