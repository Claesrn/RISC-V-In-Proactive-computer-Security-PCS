#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void win(void) {
    puts("\n win() reached -- you control the return address.");
    puts(" Spawning a shell via system(\"/bin/sh\") ...");
    fflush(stdout);
    system("/bin/sh");
    _exit(0);
}

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