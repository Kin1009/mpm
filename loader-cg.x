OUTPUT_ARCH(sh3)
OUTPUT_FORMAT(binary)
ENTRY(relocate)

MEMORY {
    /* MPM loads us at 8c700000, but we don't need a full 1 MiB of RAM.
       Relocate to 8c7f0000 so we can give a bit more to add-ins. */
    ram (rwx): o = 0x8c7f0000, l = 64k
}

SECTIONS {
    .text : {
        *(.text.reloc)
        *(.text.entry)

        _ld_bctors = . ;
        KEEP(*(.ctors .ctors.*))
        _ld_ectors = . ;

        _ld_bdtors = . ;
        KEEP(*(.dtors .dtors.*))
        _ld_edtors = . ;

        *(.text .text.*)
    } > ram

    .rodata : SUBALIGN(4) {
        *(.rodata .rodata.*)
    } > ram

    .data : {
        *(.data .data.*)
    } > ram

    .bss : ALIGN(16) {
        _ld_bbss = . ;
        *(.bss)
        . = ALIGN(16);
        _ld_ebss = . ;
    } > ram

    _reloc = ORIGIN(ram);
    _lword = ABSOLUTE(ALIGN(4) - ORIGIN(ram)) / 4;
}
