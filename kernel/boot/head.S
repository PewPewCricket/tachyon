.extern patch_relocs

.section .rodata

.section .bss @nobits
_stack_bottom:
	.skip 32768
_stack_top:
_mb2_data_ptr:
    .long 0

.section .text
.global _start
.type _start, @function
_start:
    leaq _stack_top(%rip), %rsp
    movq $0x0, %rbp
    movl %ebx, _mb2_data_ptr(%rip)
    call patch_relocs
    /* ... */
    jmp _die

.global _die
.type _die, @function
_die:
	hlt
	jmp _die