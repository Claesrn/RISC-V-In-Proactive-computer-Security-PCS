
.globl _start
_start:
    li    a7, 93          # __NR_exit
    xor   a0, a0, a0      # exit code 0
    li    t1, 0x73        # t1 = 0x00000073, the ECALL word
    addi  sp, sp, -32
    sw    t1, 16(sp)      # write the ECALL word to the stack
    addi  t2, sp, 16      # address of the word (offset >= 16 is null-free)
    jr    t2              # jump into the constructed ECALL