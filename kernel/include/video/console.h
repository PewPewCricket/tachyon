#pragma once

#include <stdint.h>

#include <video/framebuffer.h>

enum kcon_backend {
    KCON_VIDEO,
    KCON_SERIAL,
    KCON_BOTH
};

void kcon_init(struct framebuffer* fb, uint8_t port);
void kcon_set_mode(enum kcon_backend mode);
void kcon_write(const char* s);