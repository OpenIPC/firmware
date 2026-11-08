#!/usr/bin/env python3
"""Convert an Anyka EV200 "3.004" ISP tuning file to the "3.05_032401"
layout that drivers/misc/ak_isp_tuning.c accepts.

Both layouts carry the same 0x200-byte style header and the same 24
modules (IDs 0x00-0x17) with identical sizes; each style ends with a
0x1c block holding the sensor register list. The kernel additionally
wants every style to be the same size (header field 0x24) and the file
to hold exactly header[0] styles. The 3.004 files give styles of
different length (the register lists differ) and a stale style count.

Only style 0's register list is ever written to the sensor (the kernel
initialises the sensor from style 0 only), so later styles get style 0's
register list, which makes all styles the same size.
"""
import struct
import sys

HEADER = 0x200
VERSION = b"3.05_032401\0"
SIZES = [186, 192, 876, 2268, 1088, 86, 22, 876, 136, 106, 9966, 11096,
         206, 64, 6, 14, 4188, 260, 46, 528, 240, 30, 306, 996]


def styles(data):
    off = 0
    while off + HEADER + 4 <= len(data):
        mod = off + HEADER
        for i, size in enumerate(SIZES):
            mid, msize = struct.unpack_from("<HH", data, mod)
            if mid != i or msize != size:
                if i == 0:
                    return
                sys.exit(f"style at {off:#x}: module {i} is id {mid:#x} size {msize}")
            mod += size
        sid, ssize = struct.unpack_from("<HH", data, mod)
        # Unlike the modules, this size excludes the 4-byte block header.
        if sid != 0x1c or ssize % 4 or mod + 4 + ssize > len(data):
            sys.exit(f"style at {off:#x}: sensor block id {sid:#x} size {ssize}")
        yield data[off:mod], data[mod:mod + 4 + ssize]
        off = mod + 4 + ssize


def main(src, dst):
    data = open(src, "rb").read()
    found = list(styles(data))
    if not found:
        sys.exit("no styles found")
    sensor_id = struct.unpack_from("<I", data, 0x14)[0]
    regs = found[0][1]
    out = []
    for body, _ in found:
        style = bytearray(body + regs)
        struct.pack_into("<I", style, 0, len(found))
        style[4:4 + len(VERSION)] = VERSION
        struct.pack_into("<I", style, 0x24, len(style))
        if struct.unpack_from("<I", style, 0x14)[0] != sensor_id:
            sys.exit("styles disagree on the sensor id")
        out.append(bytes(style))
    open(dst, "wb").write(b"".join(out))
    print(f"{dst}: sensor {sensor_id:#x}, {len(out)} styles x {len(out[0])} bytes, "
          f"{(len(regs) - 4) // 4} sensor registers, last {regs[-4:].hex()}")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.exit(f"usage: {sys.argv[0]} vendor.conf isp.conf")
    main(sys.argv[1], sys.argv[2])
