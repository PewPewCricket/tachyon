#ifndef TACHYON_PRINTK_H
#define TACHYON_PRINTK_H

#include <stdint.h>

enum PrintkLevel : uint8_t {
    DEFAULT,
    DEBUG,
    INFO,
    NOTICE,
    WARNING,
    ERROR,
    FATAL,
};

void printk(enum PrintkLevel level, const char *fmt, ...);

#endif //TACHYON_PRINTK_H