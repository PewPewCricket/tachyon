#ifndef BOOT_DEBUG_H
#define BOOT_DEBUG_H

#ifdef KDEBUG
#define BOOT_LOG_SERIAL(s) _boot_log_serial(s);
#else
#define BOOT_LOG_SERIAL(s)
#endif

// Internal shit that shouldn't be called directly.
void _boot_log_serial(char* s);

void _boot_debug_serial_init();
char* _boot_debug_itoa(int num, char* str, const int base);
char* _boot_debug_utoa(unsigned int num, char* str, const int base);

#endif //BOOT_DEBUG_H