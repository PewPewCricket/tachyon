#include <stddef.h>

int strlen(const char* s) {
	int len = 0;
	for(;;) {
		if(s[len] == 0) {
			return len;
		}
		len++;
	}
}

int strnlen(const char* s, size_t n) {
	int len = 0;
	for(size_t i = 0; i < n; i++) {
		if(s[len] == 0) {
			return len;
		}
		len++;
	}
	return len;
}

int strcmp(const char* s1, const char* s2) {
	for (size_t i = 0;; i++) {
		if (s1[i] != s2[i]) {
			return s1[i] < s2[i] ? -1 : 1;
		} else if (s1[i] == 0 || s2[i] == 0) {
			return 0;
		}
	}
}

int strncmp(const char* s1, const char* s2, size_t n) {
	for (size_t i = 0; i < n; i++) {
		if (s1[i] != s2[i]) {
			return s1[i] < s2[i] ? -1 : 1;
		} else if (s1[i] == 0 || s2[i] == 0) {
			return 0;
		}
	}
	return 0;
}

char* strcpy(char* dest, const char* s) {
	for (size_t i = 0;; i++) {
		if (s[i] == 0) {
			return dest;
		} else {
			dest[i] = s[i];
		}
	}
}

char* strncpy(char* dest, const char* s, size_t n) {
	for (size_t i = 0; i < n; i++) {
		if (s[i] == 0) {
			return dest;
		} else {
			dest[i] = s[i];
		}
	}
	return dest;
}