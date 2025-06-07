#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <video/framebuffer.h>
#include <video/vt100.h>
#include <drivers/serial.h>

enum kcon_backend {
    KCON_VIDEO,
    KCON_SERIAL,
    KCON_BOTH
};

static enum kcon_backend kcon_mode = KCON_VIDEO;
static struct vt100_context vt100_ctx;
static uint8_t serial_port = 0; // Default to COM1

void kcon_init(struct framebuffer* fb, uint8_t port) {
    if (fb != NULL) {
        vt100_init(&vt100_ctx, fb);
        vt100_write(&vt100_ctx, "Started kernel console.\n");
    }

    serial_port = port;
    const char* test_s = "Started kernel console.\n";
    serial_write(port, (uint8_t*) test_s, strlen(test_s));
}

void kcon_set_mode(enum kcon_backend mode) {
    if (mode == KCON_VIDEO || mode == KCON_SERIAL || mode == KCON_BOTH)
        kcon_mode = mode;
}

void kcon_write(const char* s) {
    if (kcon_mode == KCON_VIDEO)
        vt100_write(&vt100_ctx, s);
    else if (kcon_mode == KCON_SERIAL)
        serial_write(serial_port, (uint8_t*) s, strlen(s));
    else if (kcon_mode == KCON_BOTH) {
        vt100_write(&vt100_ctx, s);
        serial_write(serial_port, (uint8_t*) s, strlen(s));
    }
}