# AX620E / AX630C

Status: `ax620e.lite.config` is the real working config from the
`axera-ax620e` kernel branch (openipc-linux-ax620e, branch
`axera-ax620e`) -- builds clean, produces a real `Image` + devicetree.

Target kernel version: 5.15.73, aarch64. This is not the public vendor
SDK's kernel version (4.19.125) -- it is the version a real deployed
Xiongmai IVG-A6 (AX620E) camera firmware's own kernel modules were actually
built against, read directly off their `vermagic=` string. Do not
re-derive this from the public SDK; it is wrong for this chip.

## Booting on a Xiongmai IVG-A6 (stock U-Boot 2020.04)

Flash layout: spl 64K @0, U-Boot @0x10000, env 64K @0x50000 (single
copy, CRC32), then `firmware.bin` @0x60000. The stock env boots a
squashfs `/boot/uImage` (`loadromfs ... squashfsload; axera_boot`);
OpenIPC needs, at minimum:

    setenv xmuart 0          # without it the console is muted after "Starting kernel ..."
    setenv osmem 120M        # kernel RAM; the rest (to totalmem) is ax_cmm's pool
    setenv totalmem 256M
    setenv bootargs 'mem=${osmem} console=ttyS0,115200n8 panic=20 rootfstype=squashfs root=/dev/mtdblock3 init=/init mtdparts=spi0.0:320k(boot),64k(env),<kernel>k(kernel),<rootfs>k(rootfs),-(rootfs_data)'
    setenv bootcmd 'setenv setargs setenv bootargs ${bootargs};run setargs;sf probe 0;sf read 0x45000000 0x60000 <uImage size>;bootm 0x45000000'

The stock U-Boot's console asks for a password after an interrupted
autoboot, so set these from the stock Linux (or write the env block).
The kernel has no mtdsplit, hence explicit kernel/rootfs sizes.

Known blockers (2026-11): openipc/linux axera-ax620e has no GPIO
(`axera,ax-apb-gpio`) or reset-controller (`axera,axera_reset_match`)
driver, so the NOR (CS on GPIO2_29) never probes; and the closed
`ax_*.ko` import ~175 symbols of Axera's built-in kernel OSAL
(`AX_OSAL_*`, `ax_os_mem_*`, `_ax_printk`, `ax_display_*`) that the
OpenIPC kernel does not have. No majestic for Axera exists yet.
