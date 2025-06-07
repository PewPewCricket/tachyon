#pragma once

#include <video/framebuffer.h>

int fbchar_setfb(struct framebuffer* fb); // Optionally keep for default
int fb_putc(struct framebuffer* fb, char c, uint16_t x, uint16_t y, uint32_t fg, uint32_t bg);
void fb_scroll(struct framebuffer* fb);
struct fb_info fb_get_info(struct framebuffer* fb);