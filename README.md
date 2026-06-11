# Exploring RISC-V in Proactive Computer Security (PCS)

PUK project - Department of Computer Science, University of Copenhagen

Project by Claes Refsgaard Nielsen (jvk716) & Gustav Falck Lund (sbh198)

Adapting DIKU's PCS course from **x86** to **RISC-V (RV32)** — a full
exploitation lab covering shellcode, stack attacks, and ROP, with environment
setup, vulnerable binaries, and exploit scripts.

## Quick start

Bring up the RV32 environment (pick your host):

```bash
vagrant up          # Intel / VirtualBox host
vagrant ssh

limactl start ./pcs-f2026-rv32.yaml   # Apple Silicon / Lima+QEMU host
limactl shell pcs-f2026-rv32
```

The provisioner installs the `riscv32-glibc` cross-toolchain (as `rv32-*`),
`qemu-user-static`, `gdb-multiarch`, `ROPgadget`, `checksec`, and the
`runshellcode-riscv` tool.

## Working with RISC-V

```bash
# Compiling RISC code
rv32-gcc {FILE}.s -o {FILE}            # Optional -static -nostdlib
./{FILE}

# Compiling C code
rv32-gcc {FILE}.c -o {FILE}            # Optional -static
./{FILE}

# For shellcode
rv32-gcc -nostdlib -c {FILE}.s -o {FILE}.o
rv32-objcopy -O binary -j .text {FILE}.o {FILE}.bin
runshellcode-riscv {FILE}

# Inspect
rv32-objdump -d {FILE}
readelf -x .rodata {FILE}              # See readonly data (section can be changed)

# Gdb
rv32gdb ./{FILE}

# ROPgadget
ROPgadget --binary {FILE} > {FILE}.gadgets

# file
file {FILE}

# Check mitigations
checksec --file={FILE}
```

## Running the examples

```bash
# Stack attack — overflow benign_function() and jump to win()
rv32-gcc -fno-stack-protector examples/stack/vuln.c -o vuln
python3 examples/stack/exploit.py 

# ROP chain — build execve("/bin/sh", NULL, NULL)
rv32-gcc -fno-stack-protector examples/rop/vuln.c -o vuln
python3 examples/rop/exploit.py
```

## Layout

```text
report/             LaTeX write-up (formats, calling conventions, stack diagrams)
env/                Vagrantfile + Lima YAML for the RV32 environment
examples/stack/     vulnerable C binary + Python stack-attack exploit
examples/rop/       vulnerable C binary + ROP-chain exploit
