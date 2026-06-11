#!/usr/bin/env python3
import subprocess, select, sys, struct

# Small library of functions you might find useful
##################################################

# convert 64 bit int to little endian bytes
def p64(v): return struct.pack("<Q", v)

# pause the exploit (e.g. to wait for a gdb attach)
def pause(p):
    input(f"""Run 'gdb --pid {p.pid}' in another terminal.
              Press Enter to continue.""")

def interact(p):
    """
    Let the user interact with the process
    """
    try:
        while True:
            readable, _, _ = select.select([sys.stdin.buffer, p.stdout], [], [])
            # stdin -> process
            if sys.stdin.buffer in readable:
                buf = sys.stdin.buffer.read1()
                if not buf: # stdin is closed
                    for _ in range(10):
                        readable, _, _ = select.select([p.stdout], [], [], 0.1)
                        # process -> stdout
                        if p.stdout in readable:
                            buf = p.stdout.read(4096)
                            if not buf:
                                break
                            sys.stdout.buffer.write(buf)
                            sys.stdout.flush()
                        else: break
                    break
                p.stdin.write(buf)
                p.stdin.flush()

            # process -> stdout
            if p.stdout in readable:
                buf = p.stdout.read(4096)
                if not buf:
                    break
                sys.stdout.buffer.write(buf)
                sys.stdout.flush()
    except (EOFError, KeyboardInterrupt) as e:
        pass

# read `count` bytes from the process
def readn(p, count):
    res = b''
    while len(res) < count:
        cur = p.stdout.read(count - len(res))
        if not cur:
            break
        res += cur
    return res

# read bytes from the process until a needle is found
def read_until(p, needle):
    res = b''
    while not res.endswith(needle):
        cur = readn(p, 1)
        if not cur:
            break
        res += cur
    return res

# read a line from the process
def read_line(p):
    l = read_until(p, b'\n')
    if l.endswith(b'\n'):
        return l[:-1]
    else:
        return l

# write a string to the process
def write(p, s):
    p.stdin.write(s)
    p.stdin.flush()

# write a line to the process
def write_line(p, line):
    write(p, line + b'\n')

# End of lib #####################################
