#pragma once

#include <stdint.h>

struct framebuffer {
    uint32_t* address;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t bpp;
    uint8_t red_field_pos;
    uint8_t red_mask_size;
    uint8_t green_field_pos;
    uint8_t green_mask_size;
    uint8_t blue_field_pos;
    uint8_t blue_mask_size;
}; 

struct fb_info {
    uint32_t width;
    uint32_t height;
};