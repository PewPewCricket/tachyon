#ifndef _LIBC_LIMITS_H
#define _LIBC_LIMITS_H 1

#define INT_MAX (int) (unsigned int) -1 >> 1
#define INT_MIN (-((int)((unsigned int)(~0) >> 1)) - 1)

#endif