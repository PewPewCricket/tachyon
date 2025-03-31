#include <stdint.h>
#include <string.h>
#include <boot_info.h>

#include "builtin_font.h"

// Print n bytes of s to the framebuffer at (x, y) with a specific bg and fg color
int putsnfb(char* s, uint32_t n, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg, struct fb* fb) {
    volatile uint32_t *fb_ptr = fb->addr;
    int num_printed = 0;

    for(uint32_t i = 0; i < n; i++){
        for(uint32_t j = 0; j < 16; j++) {
            for(uint32_t k = 0; k < 8; k++) {
                char bit = (font_vga8x16[s[i] * 16 + j] << k) & 0b10000000;
                if(bit) {
                    fb_ptr[(x * 8 + k) + ((y * 16 + j) * fb->width) + (8 * i)] = fg;
                } else {
                    fb_ptr[(x * 8 + k) + ((y * 16 + j) * fb->width) + (8 * i)] = bg;
                }
            }
        }
        num_printed = (int) i;
    }

    return num_printed;
}

// Print s to the framebuffer at (x, y) with a specific bg and fg color, s must end with a NULL terminator
int putsfb(char* s, uint32_t x, uint32_t y, uint32_t fg, uint32_t bg, struct fb* fb) {
    int len = strlen(s);
    int printed = putsnfb(s, (uint32_t) len, x, y, fg, bg, fb);
    return printed;
}

int writefb(char* s, struct fb* fb) {
    
}