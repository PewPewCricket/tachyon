/* Tachyon Kernel
 * Copyright (C) 2025 PewPewCricket
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/*
 * TODO: after APIC add IRQ mode
 * Use APIC timer on polling to set a timeout so an infinite loop wont happen if sender dies.
 * Functions to change baud rate
 * Functions to change line control values like data and stop bits
 */

#include <stdint.h>
#include <stdbool.h>

#include <io.h>

struct serial_port {
    uint16_t base_port;
    uint32_t baud_rate;
    uint8_t  data_bits;
    uint8_t  stop_bits;
    uint8_t  parity;
    bool     present;     // True if port is detected and usable
};

static struct serial_port ports[4];

bool probe_port(uint16_t port) {
    outb(port + 7, 0xAE);       // Test scratch register
    if (inb(port + 7) != 0xAE)
        return false;

    outb(port + 4, 0b10000);    // Enable loopback mode
    outb(port, 0xAE);
    if (inb(port) != 0xAE)
        return false;
    outb(port + 4, 0);          // Disable loopback mode

    return true;
}

void serial_init() {
    uint16_t test_ports[4] = {0x3F8, 0x2F8 , 0x3E8, 0x2E8};
    for (uint8_t i = 0; i < 4; i++) {
        ports[i].present = false;
        ports[i].base_port = test_ports[i];
        if (probe_port(test_ports[i])) {
            ports[i].present = true;
            ports[i].baud_rate = 115200;
            ports[i].data_bits = 8;
            ports[i].stop_bits = 1;
            ports[i].parity = 0;
            
            outb(test_ports[i] + 3, 0b10000000);    // Set DLAB to allow baud rate change.
            outb(test_ports[i], 1);                 // Set default baud rate,
            outb(test_ports[i] + 1, 0);             // default is 115200 baud.
            outb(test_ports[i] + 3, 0b00000011);    // Cleaar DLAB and set default values
            outb(test_ports[i] + 2, 0xC7);          // Enable FIFO
        }
    }

    return;
}

int serial_put(uint8_t port_num, uint8_t value) {
    if (port_num >= 4 || !ports[port_num].present)
        return -1;
    
    uint16_t base = ports[port_num].base_port;

    // Wait for Transmitter Holding Register to be empty (LSR bit 5)
    while ((inb(base + 5) & 0x20) == 0);

    outb(base, value);
    
    return 0;
}

int serial_write(uint8_t port_num, const uint8_t* s, uint16_t n) {
    for (uint16_t i = 0; i < n; i++) {
        int status = serial_put(port_num, s[i]);
        if (status != 0)
            return status;
    }
    return  0;
}

int serial_read(uint8_t port_num, uint8_t* out) {
    if (port_num >= 4 || !ports[port_num].present)
        return -1;

    uint16_t base = ports[port_num].base_port;
    int timeout = 1000000;
    while ((inb(base + 5) & 0x01) == 0) {
        if (--timeout == 0)
            return -2; // Timeout
    }

    uint8_t lsr = inb(base + 5);
    if (lsr & 0x1E)
        return -3; // Error

    *out = inb(base);
    return 0; // Success
}