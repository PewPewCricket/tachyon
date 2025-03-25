#include "fbio.h"
#include <string.h>
#include <limine.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>

enum log_level{
    KERN_FATAL,
    KERN_ERROR,
    KERN_WARNING,
    KERN_NOTICE,
    KERN_INFO,
    KERN_DEBUG
};

void printk(enum log_level level, char* fmt, ...) {
    //
}