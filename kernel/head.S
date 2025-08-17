.section .rodata

.section .bss @nobits
_stack_bottom:
	.skip 16384
_stack_top:

.section .text
.global _start
.type _start, @function
_start:
    jmp _die

.global _die
.type _die, @function
_die:
	hlt
	jmp _die