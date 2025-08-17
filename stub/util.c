#include <stdbool.h>
#include <stddef.h>

#include "globals.h"
#include "scrio.h"
#include "util.h"

void* memmove(void* dest, const void* src, const size_t n) {
	char* d = (char*)dest;
	const char* s = (const char*)src;

	if (d < s) {
		for (size_t i = 0; i < n; i++) {
			d[i] = s[i];
		}
	} else {
		for (size_t i = n; i > 0; i--) {
			d[i - 1] = s[i - 1];
		}
	}
	return dest;
}

int strcmp(const char *s1, const char *s2) {
	while (*s1 && (*s1 == *s2)) {
		s1++;
		s2++;
	}
	return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

static void reverse(char str[], const int length) {
	int start = 0;
	int end = length - 1;
	while (start < end) {
		const char temp = str[start];
		str[start] = str[end];
		str[end] = temp;
		end--;
		start++;
	}
}

/*
 * Copied from here: https://www.geeksforgeeks.org/c/implement-itoa/
 */
char* itoa(int num, char* str, const int base) {
	int i = 0;
	bool isNegative = false;

	/* Handle 0 explicitly, otherwise empty string is
	 * printed for 0 */
	if (num == 0) {
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	// In standard itoa(), negative numbers are handled
	// only with base 10. Otherwise, numbers are
	// considered unsigned.
	if (num < 0 && base == 10) {
		isNegative = true;
		num = -num;
	}

	// Process individual digits
	while (num != 0) {
		const int rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}

	if (isNegative)
		str[i++] = '-';
	str[i] = '\0';
	reverse(str, i);
	return str;
}

char* utoa(unsigned int num, char* str, const int base) {
	int i = 0;

	/* Handle 0 explicitly, otherwise empty string is
	 * printed for 0 */
	if (num == 0) {
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}

	// Process individual digits
	while (num != 0) {
		const int rem = num % base;
		str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
		num = num / base;
	}

	str[i] = '\0';
	reverse(str, i);
	return str;
}

_Noreturn void die() {
	if (_fb != NULL) fbprintf("!!! SYSTEM HALTED !!!\n");
	_die();
}