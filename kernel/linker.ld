ENTRY(_start)

PHDRS {
    text    PT_LOAD FLAGS(R | X);
    data    PT_LOAD FLAGS(R | W);
    rodata  PT_LOAD FLAGS(R);
}

SECTIONS {
    . = 0xffff800000000000;
    __kernel_base = .;

    .text : {
        *(.text*)
    } : text

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .rodata : {
        *(.rodata*)
    } : rodata

    .rela.dyn : {
        __reloc_start = .;
        *(.rela.dyn)
        __reloc_end = .;
    } : rodata

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .data : {
        *(.data .data.*)
    } : data

    .bss (NOLOAD) : {
        *(.*bss .*bss.* ".bss@nobits")
        *(COMMON)
    }

    __kernel_end = .;

    /DISCARD/ : {

    }
}
