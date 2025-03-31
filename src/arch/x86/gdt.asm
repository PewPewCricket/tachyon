bits 32

section .text

global gdt_init
gdt_init:
    lgdt [gdtr]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.after
.after:
    ret

section .data

global gdt
gdt:
    ; Null descriptor
    dq 0
.krnl_code:
    ; Kernel code segment
    dq 0x00FA9A000000FFFF
.krnl_data:
    ; Kernel data segment
    dq 0x00FC92000000FFFF
.user_code:
    ; Usermode code segment
    dq 0x00FAFA000000FFFF
.user_data:
    ; Usermode data segment
    dq 0x00FCF2000000FFFF
.end:

gdtr:
    dw gdt.end - gdt - 1
    dd gdt