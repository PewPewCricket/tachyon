#include <lib/ringbuf.h>
#include <drivers/fbcon.h>
#include <lib/string.h>
#include <stdarg.h>
#include <tachyon/printk.h>
#include <stdbool.h>

#define PRINTK_MAX_CHARS 32768

static enum printk_log_level loglevel = KERN_DEBUG;
static bool screen_log = true;

static RINGBUF_STATIC(printk_ringbuf, PRINTK_MAX_CHARS, char);

void printk(const enum  printk_log_level level, const char *fmt, ...) {
    if (level > loglevel) return;

    char buf[64];
    const char *p = fmt;
    va_list args;
    va_start(args, fmt);

    while (*p) {
        if (*p == '%') {
            p++;

            // Handle length modifiers
            enum { NONE, L, LL } length = NONE;
            if (*p == 'l') {
                p++;
                if (*p == 'l') {
                    length = LL;
                    p++;
                } else {
                    length = L;
                }
            }

            switch (*p) {
                case 's': {
                    const char *s = va_arg(args, char *);
                    fbcon_puts(s ? s : "(null)");
                    break;
                } case 'c': {
                    const char c = (char) va_arg(args, int);
                    buf[0] = c;
                    buf[1] = '\0';
                    fbcon_puts(buf);
                    break;
                } case 'd':
                case 'i': {
                    long long val;
                    if (length == LL) val = va_arg(args, long long);
                    else if (length == L) val = va_arg(args, long);
                    else val = va_arg(args, int);
                    strtoll(val, buf, 10);
                    fbcon_puts(buf);
                    break;
                } case 'u': {
                    unsigned long long val;
                    if (length == LL) val = va_arg(args, unsigned long long);
                    else if (length == L) val = va_arg(args, unsigned long);
                    else val = va_arg(args, unsigned int);
                    strtoull(val, buf, 10);
                    fbcon_puts(buf);
                    break;
                } case 'x':
                case 'X': {
                    unsigned long long val;
                    if (length == LL) val = va_arg(args, unsigned long long);
                    else if (length == L) val = va_arg(args, unsigned long);
                    else val = va_arg(args, unsigned int);
                    strtoull(val, buf, 16);
                    fbcon_puts(buf);
                    break;
                } case 'p': {
                    const unsigned long long val = (unsigned long long) va_arg(args, void *);
                    fbcon_puts("0x");
                    strtoull(val, buf, 16);
                    fbcon_puts(buf);
                    break;
                } case '%': {
                    fbcon_puts("%");
                    break;
                } default: {
                    buf[0] = '%';
                    buf[1] = *p;
                    buf[2] = '\0';
                    fbcon_puts(buf);
                    break;
                }
            }

            p++;
        } else {
            buf[0] = *p++;
            buf[1] = '\0';
            fbcon_puts(buf);
        }
    }

    va_end(args);
}
