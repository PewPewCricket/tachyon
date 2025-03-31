#ifndef FBIO_H
#define FBIO_H 1

#include <stdint.h>
#include <boot_info.h>

// Print n bytes of s to the framebuffer at (x, y) with a specific bg and fg color
int putsnfb(char* s, uint32_t n, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg, struct framebuffer *fb);

// Print s to the framebuffer at (x, y) with a specific bg and fg color, s must end with a NULL terminator
int putsfb(char* s, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg, struct framebuffer *fb);

#endif