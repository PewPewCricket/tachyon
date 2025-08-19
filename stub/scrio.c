#include <multiboot2.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <globals.h>
#include <font.h>
#include <string.h>

struct tty_state {
    uint16_t x;
    uint16_t y;
    uint32_t fg_color;
    uint32_t bg_color;
};

enum format_specifiers {
    FMT_NONE,
    FMT_PERCENT,
    FMT_I32,
    FMT_U32,
    FMT_H32,
    FMT_B32,
    FMT_I64,
    FMT_U64,
    FMT_H64,
    FMT_B64,
    FMT_STR
};

static struct tty_state tty_state = {0, 0, 0xFFFFFFFF, 0x00000000};

constexpr uint32_t char_width = 8;
constexpr uint32_t char_height = 16;

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
            const unsigned char bit = font[c * char_height + row] << col & 0b10000000;
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
    auto fbptr = (uint8_t*)(uintptr_t)_fb->common.framebuffer_addr;

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

void fbsetcol(const uint32_t fg_color, const uint32_t bg_color) {
    tty_state.fg_color = fg_color;
    tty_state.bg_color = bg_color;
}

void fbputs(const char *s) {
    if (!_fb || !s || *s == '\0') return;
    for (size_t i = 0; s[i] != '\0'; i++)
        fbputc(s[i]);
}

static enum format_specifiers get_specifier(const char *fmt) {
    if (*fmt != '%') return FMT_NONE;
    fmt++;
    switch (*fmt) {
        case '%': return FMT_PERCENT;
        case 's': return FMT_STR;
        case 'd': {
            if (fmt[1] == 'l') return FMT_I64;
            return FMT_I32;
        } case 'u': {
            if (fmt[1] == 'l') return FMT_U64;
            return FMT_U32;
        } case 'x': {
            if (fmt[1] == 'l') return FMT_H64;
            return FMT_H32;
        } case 'b': {
            if (fmt[1] == 'l') return FMT_B64;
            return FMT_B32;
        } default: return FMT_NONE;
    }
}

void vfbprintf(char *restrict fmt, const va_list args) {
    if (!_fb || !fmt || *fmt == '\0') return;
    char buf[128];
    for (; *fmt != '\0'; fmt++) {
        switch (get_specifier(fmt)) {
            case FMT_NONE: {
                fbputc(*fmt);
                break;
            } case FMT_PERCENT: { fmt += 1;
                fbputc('%');
                break;
            } case FMT_I32: { fmt += 1;
                const char *str = tostring(va_arg(args, int32_t), buf, 10);
                fbputs(str);
                break;
            } case FMT_I64: { fmt += 2;
                const char *str = tostring(va_arg(args, int64_t), buf, 10);
                fbputs(str);
                break;
            } case FMT_U32: { fmt += 1;
                const char *str = tostring(va_arg(args, uint32_t), buf, 10);
                fbputs(str);
                break;
            } case FMT_U64: { fmt += 2;
                const char *str = tostring(va_arg(args, uint64_t), buf, 10);
                fbputs(str);
                break;
            } case FMT_H32: { fmt += 1;
                const char *str = tostring(va_arg(args, uint32_t), buf, 16);
                fbputs(str);
                break;
            } case FMT_H64: { fmt += 2;
                const char *str = tostring(va_arg(args, uint64_t), buf, 16);
                fbputs(str);
                break;
            } case FMT_B32: { fmt += 1;
                const char *str = tostring(va_arg(args, uint32_t), buf, 2);
                fbputs(str);
                break;
            } case FMT_B64: { fmt += 2;
                const char *str = tostring(va_arg(args, uint64_t), buf, 2);
                fbputs(str);
                break;
            } case FMT_STR: { fmt += 1;
                const char *str = va_arg(args, char*);
                fbputs(str);
                break;
            }
        }
    }
}

void fbprintf(char *restrict fmt, ...) {
    if (!_fb || !fmt || *fmt == '\0') return;
    va_list args; va_start(args, fmt);
    vfbprintf(fmt, args);
    va_end(args);
}