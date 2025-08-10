#ifndef IO_H
#define IO_H
#include <stdint.h>

static inline uint8_t inb(uint16_t port) {
	uint8_t data;
	__asm__ __volatile__ ("inb %w1, %0" : "=a" (data) : "dN" (port));
	return data;
}

static inline void outb(uint16_t port, uint8_t data) {
	__asm__ __volatile__ ("outb %0, %1" : : "a"(data), "Nd"(port));
}

#endif //IO_H