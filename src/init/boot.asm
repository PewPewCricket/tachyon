bits 32

extern gdt_init
extern idt_init
extern kinit

section .multiboot

align 8
mb2_header:
    dd 0xE85250D6
    dd 0
    dd mb2_header.end - mb2_header
    dd 0x100000000 - (0xE85250D6 + 0 + (mb2_header.end - mb2_header))

    align 8
.tag_fb:
    dw 0x05
    dw 0x00
    dd .tag_end - .tag_fb
    dd 0    ; Width
    dd 0    ; Height
    dd 32   ; Depth
    align 8
.tag_end:
    dw 0
    dw 0
    dd 8
.end:

section .text

global _start:function (_start.end - _start)
_start:
    ; 16KB of stack space
    mov ebp, [stack_top]
    mov esp, [stack_top]

    ; Clear EFLAGS
    mov ecx, 0x0
    push ecx
    popfd

    ; Pass multiboot2 info to kinit
    push ebx
    push eax

    call kinit

    cli

.hang:
    hlt
    jmp .hang

.end:

section .bss

align 16
stack_bottom:
    resb 16384
stack_top: