#include <string.h>
#include <stdint.h>
#include <stddef.h>

void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
    uint8_t *restrict pdest = (uint8_t *restrict)dest;
    const uint8_t *restrict psrc = (const uint8_t *restrict)src;

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if (src > dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

static void reverse(char *str, const int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        const char x = str[start];
        str[start] = str[end];
        str[end] = x;
        start++;
        end--;
    }
}

char* strtoll(int64_t num, char *buf, const int base) {
    int i = 0;
    int is_negative = 0;

    // Negative numbers are not handled for other bases.
    if (num < 0 && base == 10) {
        is_negative = 1;
        num = -num;
    }

    if (num == 0) {
        buf[i++] = '0';
        buf[i] = '\0';
        return buf;
    }

    while (num != 0) {
        const int rem = num % base;
        buf[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    if (is_negative) {
        buf[i++] = '-';
    }

    buf[i] = '\0';
    reverse(buf, i);
    return buf;
}

char* strtoull(uint64_t num, char *buf, const int base) {
    int i = 0;

    if (num == 0) {
        buf[i++] = '0';
        buf[i] = '\0';
        return buf;
    }

    while (num != 0) {
        const int rem = num % base;
        buf[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    buf[i] = '\0';
    reverse(buf, i);
    return buf;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const uint8_t *)s1 - *(const uint8_t *)s2;
}