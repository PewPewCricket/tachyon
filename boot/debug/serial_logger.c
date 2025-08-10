/*
 * This source code is from an old version of the kernel, it should be replaced as soon as possible.
 */

#include <stdint.h>
#include <stdbool.h>
#include <tachyon/io.h>
#include <tachyon/string.h>

struct serial_port {
    uint16_t base_port;
    uint32_t baud_rate;
    uint8_t  data_bits;
    uint8_t  stop_bits;
    uint8_t  parity;
    bool     present;
};

static struct serial_port ports[4];

static bool probe_port(const uint16_t port) {
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

static void serial_put(const uint8_t port_num, const uint8_t value) {
    if (port_num >= 4 || !ports[port_num].present)
        return;

    const uint16_t base = ports[port_num].base_port;

    // Wait for Transmitter Holding Register to be empty (LSR bit 5)
    while ((inb(base + 5) & 0x20) == 0) {}

    outb(base, value);
}

void _boot_debug_serial_init() {
    for (uint8_t i = 0; i < 4; i++) {
        const uint16_t test_ports[4] = {0x3F8, 0x2F8 , 0x3E8, 0x2E8};
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
}

void serial_write(const uint8_t port_num, const uint8_t* s, uint16_t n) {
    for (uint16_t i = 0; i < n; i++)
        serial_put(port_num, s[i]);
}

void _boot_log_serial(char* s) {
    serial_write(0, (uint8_t*) s, strlen(s));
}

/* int serial_read(const uint8_t port_num, uint8_t* out) {
    if (port_num >= 4 || !ports[port_num].present)
        return -1;

    const uint16_t base = ports[port_num].base_port;
    int timeout = 1000000;
    while ((inb(base + 5) & 0x01) == 0) {
        if (--timeout == 0)
            return -2; // Timeout
    }

    const uint8_t lsr = inb(base + 5);
    if (lsr & 0x1E)
        return -3; // Error

    *out = inb(base);
    return 0; // Success
} */