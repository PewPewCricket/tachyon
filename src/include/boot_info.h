#ifndef BOOT_INFO_H
#define BOOT_INFO_H

#include <stdint.h>

struct fb {
    uint32_t* addr;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t pitch;
};

struct boot_info {
    char* loader_name;
    struct fb fb;
};

#endif