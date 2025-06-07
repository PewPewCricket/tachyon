/* Tachyon Kernel
 * Copyright (C) 2025 PewPewCricket
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

 #include <stdint.h>
 #include <string.h>

 #include <drivers/fbchar.h>
 #include <video/framebuffer.h>
 #include <video/vt100.h>

#define TAB_WIDTH 8

static inline void vt100_scroll(struct vt100_context* ctx) {
    ctx->col = 0;
    if (ctx->row >= ctx->rows)
        fb_scroll(ctx->fb);
    else
        ctx->row++;
}

static inline void vt100_ncol(struct vt100_context* ctx) {
    if (ctx->col >= ctx->cols)
        vt100_scroll(ctx);
    else
        ctx->col++;
}

void vt100_write(struct vt100_context* ctx, const char* s) {
    for (int i = 0; i < strlen(s); i++) {
        char c = s[i];
        switch (c) {
            case '\n':
                vt100_scroll(ctx);
                break;
            case '\t': {
                uint16_t next_tab = (ctx->col + TAB_WIDTH) & ~(TAB_WIDTH - 1);
                if (next_tab >= ctx->cols) {
                    vt100_scroll(ctx);
                } else {
                    ctx->col = next_tab;
                }
                break;
            }
            default:
                fb_putc(ctx->fb, c, ctx->col, ctx->row, ctx->fg, ctx->bg);
                vt100_ncol(ctx);
                break;
        }
    }
}

void vt100_init(struct vt100_context* ctx, struct framebuffer* fb) {
    ctx->fb = fb;
    struct fb_info info = fb_get_info(fb);
    ctx->cols = info.width / 8;
    ctx->rows = info.height / 16;
    ctx->row = 0;
    ctx->col = 0;
    ctx->fg = 0xAFAFAF;
    ctx->bg = 0x000000;
}