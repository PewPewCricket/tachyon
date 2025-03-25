#include <limits.h>
#include <stdbool.h>

// Taken from: https://www.geeksforgeeks.org/write-your-own-atoi/
int atoi(const char* s) {
    int sign = 1, res = 0, idx = 0;
    // Ignore leading whitespaces
    while (s[idx] == ' ') {
        idx++;
    }
    // Store the sign of number
    if (s[idx] == '-' || s[idx] == '+') {
        if (s[idx++] == '-') {
            sign = -1;
        }
    }
    // Construct the number digit by digit
    while (s[idx] >= '0' && s[idx] <= '9') {
        // Handling overflow/underflow test case
        if (res > INT_MAX / 10 || (res == INT_MAX / 10 && s[idx] - '0' > 7)) {
            return sign == 1 ? INT_MAX : INT_MIN;
        }
        // Append current digit to the result
        res = 10 * res + (s[idx++] - '0');
    }
    return res * sign;
}

// A utility function to reverse a string
void reverse(char str[], int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        end--;
        start++;
    }
}
// Implementation of itoa()
char* itoa(int num, char* str, int base) {
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
    // only with base 10. Otherwise numbers are
    // considered unsigned.
    if (num < 0 && base == 10) {
        isNegative = true;
        num = -num;
    }
 
    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }
 
    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';
 
    str[i] = '\0'; // Append string terminator
 
    // Reverse the string
    reverse(str, i);
 
    return str;
}