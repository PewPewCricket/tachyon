#pragma once
#include <limine/limine.h>

void fbcom_scroll();
void fbcon_putc(char c);
void fbcon_setcol(uint32_t fg_color, uint32_t bg_color);
void fbcon_puts(const char *s);
void fbcon_setfb(const struct limine_framebuffer *framebuffer);