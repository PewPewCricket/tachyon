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

#include <video/framebuffer.h>
#include <video/font.h>

static inline uint32_t fb_pack_pixel(
    uint8_t r, uint8_t g, uint8_t b,
    uint8_t r_size, uint8_t r_shift,
    uint8_t g_size, uint8_t g_shift,
    uint8_t b_size, uint8_t b_shift
) {
    return ((r & ((1 << r_size) - 1)) << r_shift) |
           ((g & ((1 << g_size) - 1)) << g_shift) |
           ((b & ((1 << b_size) - 1)) << b_shift);
}

int fb_putc(struct framebuffer* fb, char c, uint16_t x, uint16_t y, uint32_t fg, uint32_t bg) {
    // Color conversion
    uint8_t fg_red   = (uint8_t) ((fg >> 16) & 0xFF);
    uint8_t fg_green = (uint8_t) ((fg >> 8 ) & 0xFF);
    uint8_t fg_blue  = (uint8_t) ( fg        & 0xFF); 
    uint8_t bg_red   = (uint8_t) ((bg >> 16) & 0xFF);
    uint8_t bg_green = (uint8_t) ((bg >> 8 ) & 0xFF);
    uint8_t bg_blue  = (uint8_t) ( bg        & 0xFF); 

    uint8_t max_red   = (1 << fb->red_mask_size)   - 1;
    uint8_t max_green = (1 << fb->green_mask_size) - 1;
    uint8_t max_blue  = (1 << fb->blue_mask_size)  - 1;

    fg_red   = (fg_red   * max_red   + 127) / 255;
    fg_green = (fg_green * max_green + 127) / 255;
    fg_blue  = (fg_blue  * max_blue  + 127) / 255;
    bg_red   = (bg_red   * max_red   + 127) / 255;
    bg_green = (bg_green * max_green + 127) / 255;
    bg_blue  = (bg_blue  * max_blue  + 127) / 255;

    uint32_t fg_pixel = fb_pack_pixel(
        fg_red, fg_green,    fg_blue,
        fb->red_mask_size,   fb->red_field_pos,
        fb->green_mask_size, fb->green_field_pos,
        fb->blue_mask_size,  fb->blue_field_pos
    );
    uint32_t bg_pixel = fb_pack_pixel(
        bg_red, bg_green,    bg_blue,
        fb->red_mask_size,   fb->red_field_pos,
        fb->green_mask_size, fb->green_field_pos,
        fb->blue_mask_size,  fb->blue_field_pos
    );

    uint8_t* fbptr = (uint8_t*)fb->address;
    uint32_t bpp = fb->bpp / 8; // bytes per pixel

    for (uint32_t row = 0; row < 16; row++) {
        for (uint32_t col = 0; col < 8; col++) {
            char bit = (font_vga8x16[c * 16 + row] << col) & 0b10000000;
            uint32_t pixel = bit ? fg_pixel : bg_pixel;
            uint32_t offset = ((x * 8 + col) + ((y * 16 + row) * fb->width)) * bpp;
            for (uint32_t i = 0; i < bpp; i++) {
                fbptr[offset + i] = (pixel >> (i * 8)) & 0xFF;
            }
        }
    }

    return 0;
}

void fb_scroll(struct framebuffer* fb) {
    if (!fb)
        return;

    uint32_t char_height = 16;
    uint32_t bpp = fb->bpp / 8;
    uint32_t row_bytes = fb->width * bpp * char_height;
    uint8_t* fbptr = (uint8_t*)fb->address;

    // Move all rows up by one character height
    for (uint32_t y = 0; y < fb->height - char_height; y++) {
        uint8_t* src = fbptr + ((y + char_height) * fb->width * bpp);
        uint8_t* dst = fbptr + (y * fb->width * bpp);
        for (uint32_t x = 0; x < fb->width * bpp; x++) {
            dst[x] = src[x];
        }
    }

    // Clear the last character row
    uint32_t start = (fb->height - char_height) * fb->width * bpp;
    for (uint32_t i = 0; i < fb->width * char_height * bpp; i++) {
        fbptr[start + i] = 0;
    }
}

struct fb_info fb_get_info(struct framebuffer* fb) {
    struct fb_info info;
    info.width = fb->width;
    info.height = fb->height;
    return info;
}