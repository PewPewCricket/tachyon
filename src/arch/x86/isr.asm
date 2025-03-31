bits 32

extern isr_exception

%macro isr_err_stub 1
isr_stub_%+%1:
    call isr_exception
    iret 
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    call isr_exception
    iret
%endmacro

section .text

isr_stub_0:
    push 0x0
    call isr_exception
    iret
isr_stub_1:
    push 0x1
    call isr_exception
    iret
isr_stub_2:
    push 0x2
    call isr_exception
    iret
isr_stub_3:
    push 0x3
    call isr_exception
    iret
isr_stub_4:
    push 0x4
    call isr_exception
    iret
isr_stub_5:
    push 0x5
    call isr_exception
    iret
isr_stub_6:
    push 0x6
    call isr_exception
    iret
isr_stub_7:
    push 0x7
    call isr_exception
    iret
isr_stub_8:
    push 0x8;
    call isr_exception
    iret
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

section .data

global isr_stub_table
isr_stub_table:
%assign i 0 
%rep    32 
    dd isr_stub_%+i
%assign i i+1 
%endrep