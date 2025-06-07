#include <stdint.h>

uint32_t u32pow(uint8_t base, uint8_t exponent) {
    int result = 1;
    for (uint8_t i = 0; i < exponent; i++) {
        result *= base;
    }
    return result;
}