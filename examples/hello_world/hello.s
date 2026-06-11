# hello_rv.s - RISC-V (RV64) Linux, no libc
# Assemble & link:
#   riscv64-linux-gnu-as hello_rv.s -o hello_rv.o
#   riscv64-linux-gnu-ld hello_rv.o -o hello_rv
# Run:
#   qemu-riscv64-static ./hello_rv

    .section .rodata
msg:
    .ascii "Hello from RISC-V assembly!\n"
    .equ msglen, . - msg

    .section .text
    .globl _start
_start:
    # write(1, msg, msglen)
    li      a0, 1           # fd = stdout
    la      a1, msg         # buf
    li      a2, msglen      # count
    li      a7, 64          # syscall number for write
    ecall

    # exit(0)
    li      a0, 0           # status
    li      a7, 93          # syscall number for exit
    ecall