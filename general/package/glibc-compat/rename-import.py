#!/usr/bin/env python3
"""
rename-import.py <lib.so> <old> <new> -- point one import of a vendor .so at
a libglibc-compat entry point.

Some glibc calls cannot be bridged by a library both ABIs share: a vendor
blob calling lseek() passes glibc's 32-bit off_t in r1 and whence in r2,
musl code passes a 64-bit off_t in r2:r3 and whence on the stack, and an
lseek() interposed for everyone cannot tell which caller it has. Renaming
the blob's own import sends just that blob to a 32-bit wrapper and leaves
every musl caller on musl's lseek.

Buildroot's patchelf (0.13) predates --rename-dynamic-symbols, and .dynstr
cannot grow without relinking, so the name is rewritten in place: it must be
the same length, belong to an undefined symbol, and own its bytes outright
(no other symbol, version or dynamic entry may point into them, and they may
not be the tail of a longer string). Anything else is refused. Running it
again on a renamed library is a no-op.
"""

import struct
import sys

SHT_DYNSYM = 11
SHT_DYNAMIC = 6
SHT_GNU_VERNEED = 0x6ffffffe
SHT_GNU_VERDEF = 0x6ffffffd
DT_STRTAB_REFS = {1, 14, 15, 29}  # DT_NEEDED, DT_SONAME, DT_RPATH, DT_RUNPATH


def fail(msg):
    sys.exit(f"rename-import: {msg}")


def main():
    if len(sys.argv) != 4:
        fail("usage: rename-import.py <lib.so> <old> <new>")
    path, old, new = sys.argv[1], sys.argv[2].encode(), sys.argv[3].encode()
    if len(old) != len(new):
        fail(f"{old.decode()} and {new.decode()} differ in length")

    data = bytearray(open(path, "rb").read())
    if data[:4] != b"\x7fELF" or data[4] != 1 or data[5] != 1:
        fail(f"{path}: not a little-endian ELF32 file")

    shoff, = struct.unpack_from("<I", data, 0x20)
    shentsize, shnum = struct.unpack_from("<HH", data, 0x2e)
    sections = [struct.unpack_from("<IIIIIIIIII", data, shoff + i * shentsize)
                for i in range(shnum)]

    dynsym = next((s for s in sections if s[1] == SHT_DYNSYM), None)
    if not dynsym:
        fail(f"{path}: no .dynsym")
    strtab = sections[dynsym[6]]
    str_off, str_size = strtab[4], strtab[5]

    # Every offset into .dynstr that something else refers to.
    refs = []
    symbols = []
    for i in range(dynsym[5] // 16):
        name, _value, _size, _info, _other, shndx = struct.unpack_from(
            "<IIIBBH", data, dynsym[4] + i * 16)
        symbols.append((name, shndx))
        refs.append(name)
    for s in sections:
        if s[1] == SHT_DYNAMIC:
            for j in range(s[5] // 8):
                tag, val = struct.unpack_from("<iI", data, s[4] + j * 8)
                if tag in DT_STRTAB_REFS:
                    refs.append(val)
        elif s[1] == SHT_GNU_VERNEED:
            off = s[4]
            while True:
                _ver, cnt, file_, aux, nxt = struct.unpack_from("<HHIII", data, off)
                refs.append(file_)
                a = off + aux
                for _ in range(cnt):
                    _hash, _flags, _other, name, anext = struct.unpack_from("<IHHII", data, a)
                    refs.append(name)
                    a += anext
                if not nxt:
                    break
                off += nxt
        elif s[1] == SHT_GNU_VERDEF:
            off = s[4]
            while True:
                _ver, _flags, _ndx, cnt, _hash, aux, nxt = struct.unpack_from("<HHHHIII", data, off)
                a = off + aux
                for _ in range(cnt):
                    name, anext = struct.unpack_from("<II", data, a)
                    refs.append(name)
                    a += anext
                if not nxt:
                    break
                off += nxt

    def name_at(off):
        end = data.index(b"\0", str_off + off)
        return bytes(data[str_off + off:end])

    def find(name):
        return [(n, shndx) for n, shndx in symbols if name_at(n) == name]

    if find(new) and not find(old):
        return  # already renamed
    hits = find(old)
    if len(hits) != 1:
        fail(f"{path}: expected one {old.decode()} symbol, found {len(hits)}")
    off, shndx = hits[0]
    if shndx != 0:
        fail(f"{path}: {old.decode()} is defined here, not imported")
    if off == 0 or data[str_off + off - 1] != 0:
        fail(f"{path}: {old.decode()} is the tail of a longer string")
    if any(off < r < off + len(old) for r in refs) or refs.count(off) != 1:
        fail(f"{path}: another entry shares the bytes of {old.decode()}")
    if off + len(old) >= str_size:
        fail(f"{path}: {old.decode()} runs past .dynstr")

    data[str_off + off:str_off + off + len(new)] = new
    open(path, "wb").write(data)


if __name__ == "__main__":
    main()
