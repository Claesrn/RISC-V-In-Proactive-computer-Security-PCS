#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char binsh[16] = "/bin/sh";

void benign_function(void) {
    char buf[64];
    puts("Enter your input:");
    fflush(stdout);

    /* THE BUG: reads far more than buf can hold. */
    ssize_t n = read(0, buf, 0x200);
    (void)n;

    printf("You said: %s\n", buf);
    fflush(stdout);
}

int main(void) {
    setvbuf(stdin,  NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);

    puts("== RISC-V ROP playground ==");
    benign_function();
    puts("benign_function returned normally -- no overflow triggered.");
    return 0;
}

// Gadgets that match the exact instruction sequences used in the ROP chain.
// Inspiration from https://www.bogdandeac.com/return-oriented-programming-on-risc-v/
__asm__(
    ".text\n"
    ".balign 4\n"
    ".globl charger\n"
    ".globl g_set_a0\n"
    ".globl g_zero_a1a2\n"
    ".globl g_set_a7\n"
    ".globl g_syscall\n"
 
    /* ---- linker gadget: load ra + s0..s5 from the stack, bump sp, ret ---- */
    "linker:\n"
    "    lw   ra, 0(sp)\n"
    "    lw   s0, 4(sp)\n"
    "    lw   s1, 8(sp)\n"
    "    lw   s2, 12(sp)\n"
    "    lw   s3, 16(sp)\n"
    "    lw   s4, 20(sp)\n"
    "    lw   s5, 24(sp)\n"
    "    addi sp, sp, 28\n"
    "    ret\n"                 /* = jalr zero, 0(ra) -> jumps to g_set_a0 */
 
    /* ---- functional gadgets, each ends in jalr sN to chain forward ------- */
    "g_set_a0:\n"
    "    mv   a0, s0\n"         /* a0 = &binsh */
    "    jalr s1\n"             /* -> g_zero_a1a2 */
 
    "g_zero_a1a2:\n"
    "    mv   a1, s2\n"         /* a1 = 0 */
    "    mv   a2, s2\n"         /* a2 = 0 */
    "    jalr s3\n"             /* -> g_set_a7 */
 
    "g_set_a7:\n"
    "    mv   a7, s4\n"         /* a7 = 221 (execve) */
    "    jalr s5\n"             /* -> g_syscall */
 
    "g_syscall:\n"
    "    ecall\n"               /* execve("/bin/sh", 0, 0) */
);