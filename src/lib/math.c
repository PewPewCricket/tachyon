#include <stdint.h>

int64_t pow(const int base, const int exponent) {
    int64_t result = 1;

    if (exponent < 0)
        return 0;

    for (int i = 0; i < exponent; i++)
        result *= base;

    return result;
}