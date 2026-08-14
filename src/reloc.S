.section .text.reloc
.global relocate

relocate:
    mov.l   .origin, r0
    mov.l   .reloc, r1
    mov.l   .lword, r2

1:  mov.l   @r0+, r3
    mov.l   r3, @r1
    ocbwb   @r1
    synco
    icbi    @r1
    dt      r2
    bf.s    1b
    add     #4, r1

    mov.l   .start, r0
    jmp     @r0
    nop

.balign 4
.origin:
    .long 0x8c700000
.reloc:
    .long _reloc
.lword:
    .long _lword
.start:
    .long _start
