//
// Created by pewpewcricket on 8/16/25.
//

#ifndef SCRIO_H
#define SCRIO_H
#include <stdarg.h>
#include <stdint.h>

void fbscroll();
void fbputc(char c);
void fbsetcol(uint32_t fg_color, uint32_t bg_color);
void fbputs(char *s);
void fbprintf(char *restrict fmt, ...);
void vfbprintf(char *restrict fmt, va_list args);

#endif //SCRIO_H
