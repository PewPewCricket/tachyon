ENTRY(_start)

PHDRS {
    mb2     PT_LOAD FLAGS(R);
    text    PT_LOAD FLAGS(R | X);
    data    PT_LOAD FLAGS(R | W);
    rodata  PT_LOAD FLAGS(R);
}

SECTIONS {
    . = 4M;
    __STUB_BASE = .;

    .mb2 : ALIGN(8) {
        *(.mb2*)
    } : mb2

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .text : {
        *(.text*)
    } : text

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .rodata : {
        *(.rodata*)
    } : rodata

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .data : {
        *(.data .data.*)
    } : data

    .bss (NOLOAD) : {
        *(.*bss .*bss.* ".bss@nobits")
        *(COMMON)
    }

    __STUB_END = .;
}
