    .arm
    .text

    .global _start
@---------------------------------------------------------------------------------
_start:
    mov r12, #0x2bc000
    bx r12