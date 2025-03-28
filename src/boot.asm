bits 32

section .multiboot_header
mb2_header_start:
    ; Multiboot header
    dd 0xE85250D6
    dd 0
    dd mb2_header_end - mb2_header_start
    dd 0x100000000 - (0xE85250D6 + 0 + (mb2_header_end - mb2_header_start))

    ; Multiboot tags
mb2_tag_fb_start:
    dw 0x05
    dw 0x00
    dd mb2_tag_fb_end - mb2_tag_fb_start
    dd 0    ; Width
    dd 0    ; Height
    dd 32   ; Depth
mb2_tag_fb_end:

    ; End tag
    dw 0
    dw 0
    dd 8
mb2_header_end:

global start

section .text
start:
.loop:
    jmp .loop

section .data
gdt_start:
    ; Null descriptor
    dq 0
    
    ; Kernel code segment
    dq 0xFFFF0000009AFA00

    ; Kernel data segment
    dq 0xFFFF00000092FC00

    ; Usermode code segment
    dq 0xFFFF000000FAFA00

    ; Usermode data segment
    dq 0xFFFF000000F2FC00
gdt_end:

section .bss
align 16
stack_bottom:
    times 16384 db 0