# hello_rv.s - RISC-V (RV64) Linux, dynamically linked against libc
# Assemble, link (dynamic) via gcc:
#   riscv64-linux-gnu-gcc hello_rv.s -o hello_rv
# Run (point QEMU at the RISC-V loader + libs):
#   qemu-riscv64-static -L /usr/riscv64-linux-gnu ./hello_rv

    .section .rodata
msg:
    .ascii "Hello from RISC-V assembly!\n"
    .equ msglen, . - msg

    .section .text
    .globl main
main:
    addi    sp, sp, -16         # prologue: save return address
    sw      ra, 8(sp)

    # write(1, msg, msglen)
    li      a0, 1               # fd = stdout
    la      a1, msg             # buf
    li      a2, msglen          # count
    call    write               # libc write() -> resolved via PLT/GOT

    # exit(0)
    li      a0, 0               # status
    call    exit                # libc exit() -> resolved via PLT/GOT

    # not reached, but keep a clean epilogue
    lw     ra, 8(sp)
    addi    sp, sp, 16
    ret