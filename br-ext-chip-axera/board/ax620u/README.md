# AX620U

Axera AX620U, 4x Cortex-A7 (AArch32), 128 MiB SPI NAND. Reference board:
JFtech IPC_AX620U_A8 (OS04A10 sensor on i2c8).

## Kernel

OpenIPC/linux `axera-ax620u`: Linux 4.19.125 with Axera's SDK kernel
(maix_ax620e_sdk) on top, plus AX620U support (`ax620u.dts` from the stock
FIT, a fixed-rate clock provider, the `axera,ax620x-smp` holding pen, the
printk-style `ax_printk` the closed modules call, 2-channel AXI DMAC, PWM).
`ax620u.lite.config` is the stock camera kernel's own config moved onto that
tree, so the stock `ax_*.ko`/`Xm*.ko` in `axera-osdrv-ax620u` load unchanged
(vermagic `4.19.125 SMP preempt mod_unload ARMv7 p2v8`).

## Flash layout: OpenIPC NAND

| offset    | size      | mtd | content                                        |
|-----------|-----------|-----|------------------------------------------------|
| 0x000000  | 1 MiB     | 0   | spl: vendor SPL (DDR init), kept as shipped    |
| 0x100000  | 1.5 MiB   | 1   | boot: OpenIPC U-Boot (u-boot `openipc-ax620u`) |
| 0x280000  | 512 KiB   | 2   | env                                            |
| 0x300000  | 125 MiB   | 3   | ubi: `rootfs` (UBIFS, /boot/fitImage), `rootfs_data` |

`mtdparts=spi4.0_nand:1024k(spl),1536k(boot),512k(env),-(ubi)`,
`root=ubi0:rootfs rootfstype=ubifs ubi.mtd=3,2048`; U-Boot boots with
`ubi part ubi; ubifsmount ubi0:rootfs; ubifsload ${baseaddr} /boot/fitImage;
bootm`. The kernel FIT comes from `nand-fit.its` (rootfs_script.sh, NAND FIT
step), the UBI image from `ubinize-nand.cfg`; the package carries
fitImage, rootfs.ubifs and rootfs.ubi, and sysupgrade works as on the other
NAND boards. `mknand.py` assembles a whole-flash image (vendor SPL from a
stock dump + U-Boot + rootfs.ubi) for QEMU or a programmer.

The stock Xiongmai U-Boot is not used: it has ubi/ubifs commands, but it
password-protects its prompt, and OpenIPC ships its own U-Boot per family.

## Tested

In QEMU only (OpenIPC/qemu-bench `-M ax620u -smp 4`), from `mknand.py`'s
whole-flash image (vendor SPL, OpenIPC U-Boot, this build's `rootfs.ubi`):

- U-Boot boots `/boot/fitImage` from UBIFS; kernel `4.19.125` on 4 CPUs,
  `root=ubi0:rootfs`, `ubi0:rootfs_data` (UBIFS) on `/overlay`, overlayfs
  on `/`;
- first login sets the root password; `ipcinfo -v/-c/-f` print
  `axera`/`ax620u`/`ax620u`; `S70vendor` runs `load_axera`: all 28
  modules in `modules.dep` load, no failure logged, no oops;
- a file written to `/etc` and the password survive a reboot.

Real hardware has not run this image yet.

## Not yet

- The AX620U clock driver is not in the SDK: every clock is a fixed 24 MHz
  stand-in, which leaves the hardware as the bootloader set it up.
- No streamer: majestic has no Axera build.
