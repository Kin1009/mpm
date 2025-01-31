OUTPUT_ARCH(sh3)
OUTPUT_FORMAT(binary)
ENTRY(_start)

MEMORY {
    ram (rwx): o = 0x8c700000, l = 1M
}

SECTIONS {
    .text : {
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
}
