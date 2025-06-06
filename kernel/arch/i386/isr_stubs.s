.section .text

.macro ISR_ERR_STUB n
isr_stub_\n:
    push \n
    call exception_handler
    iret
.endm

.rept 32
ISR_ERR_STUB \+
.endr

.global isr_stub_table
isr_stub_table:
.rept 32
.long isr_stub_\+
.endr