#ifndef PRINTK_H
#define PRINTK_H 1

enum log_level{
    KERN_FATAL,
    KERN_ERROR,
    KERN_WARNING,
    KERN_NOTICE,
    KERN_INFO,
    KERN_DEBUG
};

void printk(enum log_level level, char* fmt, ...);

#endif