#include <multiboot2.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include "globals.h"
#include "font.h"
#include "util.h"

struct tty_state {
    uint16_t x;
    uint16_t y;
    uint32_t fg_color;
    uint32_t bg_color;
};

static struct tty_state tty_state = {0, 0, 0xFFFFFFFF, 0x00000000};

const uint32_t char_width = 8;
const uint32_t char_height = 16;

static inline uint32_t fb_pack_pixel(
    const uint8_t r, const uint8_t g, const uint8_t b,
    const uint8_t r_size, const uint8_t r_shift,
    const uint8_t g_size, const uint8_t g_shift,
    const uint8_t b_size, const uint8_t b_shift
) {
    return ((r & ((1 << r_size) - 1)) << r_shift) |
           ((g & ((1 << g_size) - 1)) << g_shift) |
           ((b & ((1 << b_size) - 1)) << b_shift);
}

static void fbset(const char c, const uint16_t x, const uint16_t y, const uint32_t fg, const uint32_t bg) {
    if (!_fb) return;

    // Color conversion
    uint8_t fg_red   = (uint8_t) ((fg >> 16) & 0xFF);
    uint8_t fg_green = (uint8_t) ((fg >> 8 ) & 0xFF);
    uint8_t fg_blue  = (uint8_t) ( fg        & 0xFF);
    uint8_t bg_red   = (uint8_t) ((bg >> 16) & 0xFF);
    uint8_t bg_green = (uint8_t) ((bg >> 8 ) & 0xFF);
    uint8_t bg_blue  = (uint8_t) ( bg        & 0xFF);

    const uint8_t max_red   = (1 << _fb->framebuffer_red_mask_size)   - 1;
    const uint8_t max_green = (1 << _fb->framebuffer_green_mask_size) - 1;
    const uint8_t max_blue  = (1 << _fb->framebuffer_blue_mask_size)  - 1;

    fg_red   = (fg_red   * max_red   + 127) / 255;
    fg_green = (fg_green * max_green + 127) / 255;
    fg_blue  = (fg_blue  * max_blue  + 127) / 255;
    bg_red   = (bg_red   * max_red   + 127) / 255;
    bg_green = (bg_green * max_green + 127) / 255;
    bg_blue  = (bg_blue  * max_blue  + 127) / 255;

    const uint32_t fg_pixel = fb_pack_pixel(
        fg_red, fg_green, fg_blue,
        _fb->framebuffer_red_mask_size,   _fb->framebuffer_red_field_position,
        _fb->framebuffer_green_mask_size, _fb->framebuffer_green_field_position,
        _fb->framebuffer_blue_mask_size,  _fb->framebuffer_blue_field_position
    );

    const uint32_t bg_pixel = fb_pack_pixel(
        bg_red, bg_green, bg_blue,
        _fb->framebuffer_red_mask_size,   _fb->framebuffer_red_field_position,
        _fb->framebuffer_green_mask_size, _fb->framebuffer_green_field_position,
        _fb->framebuffer_blue_mask_size,  _fb->framebuffer_blue_field_position
    );

    uint8_t* fbptr = (uint8_t*)(uintptr_t)_fb->common.framebuffer_addr;
    const uint32_t bpp = _fb->common.framebuffer_bpp / 8; // bytes per pixel

    for (uint32_t row = 0; row < char_height; row++) {
        for (uint32_t col = 0; col < char_width; col++) {
            const char bit = font[c * char_height + row] << col & 0b10000000;
            const uint32_t pixel = bit ? fg_pixel : bg_pixel;
            const uint32_t offset = ((x * char_width + col) + ((y * char_height + row) * _fb->common.framebuffer_width)) * bpp;
            for (uint32_t i = 0; i < bpp; i++) {
                fbptr[offset + i] = pixel >> (i * char_width) & 0xFF;
            }
        }
    }
}

void fbscroll() {
    if (!_fb) return;

    const uint32_t bpp = _fb->common.framebuffer_bpp / 8; // bytes per pixel
    const uint32_t row_bytes = _fb->common.framebuffer_width * bpp * char_height;
    uint8_t* fbptr = (uint8_t*)(uintptr_t)_fb->common.framebuffer_addr;

    // Move all rows up by one character height
    memmove(fbptr, fbptr + row_bytes, (_fb->common.framebuffer_height - char_height) * row_bytes);

    // Clear the last character row
    const uint32_t start = (_fb->common.framebuffer_height - char_height) * _fb->common.framebuffer_width * bpp;
    for (uint32_t i = 0; i < row_bytes; i++) {
        fbptr[start + i] = 0;
    }
}

void fbputc(const char c) {
    if (!_fb) return;
    const uint32_t max_x = _fb->common.framebuffer_width / char_width;
    const uint32_t max_y = _fb->common.framebuffer_height / char_height;

    switch (c) {
        case '\n':
            tty_state.x = 0;
            tty_state.y++;
            break;
        case '\t':
            tty_state.x = (tty_state.x + 8) & ~7;
            break;
        default:
            fbset(c, tty_state.x, tty_state.y, tty_state.fg_color, tty_state.bg_color);
            tty_state.x++;
    }

    // Handle line wrapping
    if (tty_state.x >= max_x) {
        tty_state.x = 0;
        tty_state.y++;
    }

    // Handle scrolling if we have written past the screen height
    if (tty_state.y >= max_y) {
        fbscroll();
        tty_state.y = max_y - 1;
    }
}

void fbputs(const char *s) {
    if (!_fb || !s || *s == '\0') return;
    for (size_t i = 0; s[i] != '\0'; i++) {
        fbputc(s[i]);
    }
}

void fbprintf(char *restrict fmt, ...) {
    if (!_fb || !fmt || *fmt == '\0') return;
    va_list args; va_start(args, fmt);
    for (size_t i = 0; fmt[i] != '\0'; i++) {
        if (fmt[i] != '%') {
            fbputc(fmt[i]);
            continue;
        }
        const char sp = fmt[i + 1];
        char buf[64];
        switch (sp) {
            case '%': {
                fbputc('%');
                break;
            } case 's': {
                fbputs(va_arg(args, char*));
                break;
            } case 'd': {
                const char *str = itoa(va_arg(args, int), buf, 10);
                fbputs(str);
                break;
            } case 'u': {
                const char *str = utoa(va_arg(args, unsigned int), buf, 10);
                fbputs(str);
                break;
            } default:
                break;
        }
        i += 1;
    }
    va_end(args);
}