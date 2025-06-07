#pragma once

struct vt100_context {
    struct framebuffer* fb;
    uint16_t row, col;
    uint16_t rows, cols;
    uint32_t fg, bg;
};

void vt100_init(struct vt100_context* ctx, struct framebuffer* fb);
void vt100_write(struct vt100_context* ctx, const char* s);