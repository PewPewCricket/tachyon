/* Tachyon Kernel
 * Copyright (C) 2025 PewPewCricket
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

# The funny dogshit code that sets everything up

.section .multiboot
.align 8

mb2h:
    .long 0xE85250D6
    .long 0
    .long mb2h_end - mb2h
    .long 0x100000000 - (0xE85250D6 + 0 + (mb2h_end - mb2h))

.align 8
mb2h_tag_fb:
    .word 5
    .word 0
    .long mb2h_tag_end - mb2h_tag_fb
    .long 0    # Framebuffer width
    .long 0    # Framebuffer height
    .long 32   # Framebuffer depth

.align 8
mb2h_tag_end:
    .word 0
    .word 0
    .long 8
mb2h_end:

.section .bss

# 16KB of stack space
.align 16
stack_bottom:
.skip 16384
stack_top:


.section .text

.extern boot

.global _start
.type _start, @function
_start:
    # setup stack 
    mov $stack_top, %ebp
    mov $stack_top, %esp

    pushl %ebx
    call boot
