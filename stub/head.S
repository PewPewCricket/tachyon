/* linker script symbols */
.extern __STUB_BASE
.extern __STUB_END

#define ASM_FILE 1
#include <multiboot2.h>
#include "parse.h"

.section .mb2
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
	.long 20
	.long 0
	.long 0
	.long 32

.align 8
mb2h_tag_module_align:
	.word 6
	.word 0
	.long 8

.align 8
mb2h_tag_mbi:
	.word 1
	.word 0
	.long mb2h_tag_end - mb2h_tag_mbi
	.long 3
	.long 6
	.long 8
	.long 15
	.long 21

.align 8
mb2h_tag_end:
	.word 0
	.word 0
	.long 8
mb2h_end:

.section .rodata

.section .bss @nobits
_stack_bottom:
	.skip 16384
_stack_top:

.align 4
.global _mb2_data_ptr
_mb2_data_ptr:
	.long 0

.section .text
.global _start
.type _start, @function
_start:
    cmpl $0x36d76289, %eax
	jne _die

    movl $_stack_top, %esp
    movl %esp, %ebp

    movl %ebx, _mb2_data_ptr
    push %ebx
    call mb2_parse_tags

    call die

.global _die
.type _die, @function
_die:
	hlt
	jmp _die