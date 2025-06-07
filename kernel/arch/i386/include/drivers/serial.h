#pragma once

#include <stdint.h>

void serial_init();
int serial_write(uint8_t port_num, const uint8_t* s, uint16_t n);
int serial_read(uint8_t port_num, uint8_t* out);