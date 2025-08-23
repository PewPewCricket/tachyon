#ifndef GLOBALS_H
#define GLOBALS_H
#include <stdint.h>
#include <multiboot2.h>
#include <stddef.h>

#define STACK_CHK_GUARD 0xe2dee396

extern char __STUB_BASE;
extern char __STUB_END;

extern void *_kernel_ptr;
extern uint32_t _kernel_size;

extern struct multiboot_tag_framebuffer *_fb;

#endif //GLOBALS_H
