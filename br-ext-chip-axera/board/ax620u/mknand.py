#!/usr/bin/env python3
"""mknand.py <u-boot-ax620u-nand.bin> <rootfs.ubi> <out.dump>

A whole-flash AX620U SPI-NAND image (128 MiB, raw, no OOB) in OpenIPC's NAND
layout, 768k(boot),256k(env),-(ubi), for QEMU or a programmer:

  0x000000  768 KiB   boot  u-boot-ax620u-nand.bin (OpenIPC/u-boot, release
                            `axera`): the first stage at 0 and 0x20000,
                            U-Boot at 0x40000
  0x0c0000  256 KiB   env   erased: U-Boot starts on its default environment
  0x100000  127 MiB   ubi   rootfs.ubi (rootfs + rootfs_data), erased past it;
                            UBI grows rootfs_data (autoresize) on first attach
"""
import sys

BOOT, ENV, UBI, SIZE = 0, 0xc0000, 0x100000, 128 << 20
MAGIC = bytes.fromhex('22335455')

boot, ubi, out = sys.argv[1:4]
b = open(boot, 'rb').read()
assert len(b) <= ENV - BOOT, 'the U-Boot image does not fit the boot partition'
assert (b[4:8] == MAGIC and b[:0x400] == b[0x400:0x800]
        and b[0x40004:0x40008] == MAGIC), \
    'not an AX620U image of OpenIPC\'s layout (first stage, U-Boot at 0x40000)'
u = open(ubi, 'rb').read()
assert len(u) <= SIZE - UBI, 'rootfs.ubi too big'
img = bytearray(b'\xff' * SIZE)
img[BOOT:BOOT + len(b)] = b
img[UBI:UBI + len(u)] = u
open(out, 'wb').write(img)
