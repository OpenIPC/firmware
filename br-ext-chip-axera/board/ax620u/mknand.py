#!/usr/bin/env python3
"""mknand.py <spl-source.dump> <u-boot-ax620u-nand.bin> <rootfs.ubi> <out.dump>

A whole-flash AX620U SPI-NAND image (128 MiB, raw, no OOB) in OpenIPC's NAND
layout, for QEMU or a programmer:

  0x000000  1 MiB     spl   the vendor SPL (DDR init), taken from a stock dump
  0x100000  1.5 MiB   boot  OpenIPC U-Boot, with its Axera image header
  0x280000  512 KiB   env   erased: U-Boot starts on its default environment
  0x300000  125 MiB   ubi   rootfs.ubi (rootfs + rootfs_data), erased past it;
                            UBI grows rootfs_data (autoresize) on first attach
"""
import sys
spl_src, boot, ubi, out = sys.argv[1:5]
d = open(spl_src, 'rb').read()
assert len(d) == 128 << 20, 'not a 128 MiB dump'
img = bytearray(b'\xff' * len(d))
img[:0x100000] = d[:0x100000]
b = open(boot, 'rb').read()
assert len(b) <= 0x180000, 'U-Boot image too big'
img[0x100000:0x100000 + len(b)] = b
u = open(ubi, 'rb').read()
assert len(u) <= len(img) - 0x300000, 'rootfs.ubi too big'
img[0x300000:0x300000 + len(u)] = u
open(out, 'wb').write(img)
