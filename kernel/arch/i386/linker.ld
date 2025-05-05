OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

PHDRS {
    text PT_LOAD;
    data PT_LOAD;
    rodata PT_LOAD;
}

SECTIONS
{
	. = 2M;

    .text BLOCK(4K) : ALIGN(4K) {
        KEEP(*(.multiboot));
        *(.text)
    } : text

	/* Read-only data. */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    } :rodata

    /* Read-write data (initialized) */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    } :data

    /* Read-write data (uninitialized) and stack */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    } :data

	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply add stuff here as needed. */
}
