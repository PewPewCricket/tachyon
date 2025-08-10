#ifdef KDEBUG
#ifndef BOOT_DEBUG_H
#define BOOT_DEBUG_H

void serial_init();
void boot_log_serial(char* s);

#endif //BOOT_DEBUG_H
#endif // KDEBUG