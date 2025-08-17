ENTRY(_start)

PHDRS {
    text    PT_LOAD FLAGS(R | X);
    data    PT_LOAD FLAGS(R | W);
    rodata  PT_LOAD FLAGS(R);
}

SECTIONS {
    . = 0;
    __KERNEL_BASE = .;

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

    __KERNEL_END = .;

    /DISCARD/ : {
        *(*)
    }
}
