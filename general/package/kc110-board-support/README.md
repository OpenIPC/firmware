# kc110-board-support

Buildroot package providing hardware support for the **TP-Link Kasa KC110** PTZ camera (HiSilicon Hi3518EV200 + OV2735 MIPI sensor + 24BYJ48 stepper motors driven by a ULN2803 darlington array).

## What it installs

| File | Purpose |
|---|---|
| `/usr/bin/ptz` | Small C binary that drives pan/tilt steppers (via PL061 mmap) and IR-cut/IR-LED solenoids. Uses the vendor's "base-10" GPIO numbering, which on Hi3518EV200 happens to map cleanly onto OpenIPC's existing PL061 chips (`bank = N/10`, `pin = N%10`). |
| `/usr/bin/gpio-motors` | 451-byte shell shim that forwards `gpio-motors X Y N` calls (used by motor.cgi in the web UI) to `ptz pan/tilt`. Replaces the upstream `gpio-motors` binary, which uses sysfs and a different pin-numbering convention that does not match this board. |
| `/usr/bin/ir-control.sh` | Convenience wrapper for the IR-LED + IR-cut filter. |
| `/etc/init.d/S94pinmux-pan-ir` | Boot-time init that writes `0x1` to pinmux registers `0x200f00e8/ec/f0/fc/100/104`. The KC110's pan + IR-cut + IR-LED pads (chip7/chip8 region) need this; the kernel pinctrl driver does not set it automatically, and a soft reboot does not clear the SoC's pinmux state. Without this script, motor/solenoid GPIO writes succeed silently but the pads stay in the wrong function — looking 100 % like a hardware fault. |

## KC110 GPIO map (vendor base-10 → OpenIPC PL061 pin)

```
pan motor coils A/B/C/D : 65 / 60 / 59 / 58   (vendor: 81/74/73/72)
tilt motor coils A/B/C/D: 0 / 1 / 2 / 36      (vendor: 0/1/2/44)
IR-cut filter solenoid  : 64                  (vendor: 80)
IR-LED                  : 63                  (vendor: 77)
```

Note that vendor pin `44` translates to OpenIPC pin `36` (chip4 base 32 + pin 4), not pin `44` as the naive reading would suggest.

## Conflicts

This package installs `/usr/bin/gpio-motors` and therefore conflicts with the existing `BR2_PACKAGE_GPIO_MOTORS` package, which installs a sysfs-based binary at the same path. Enable only one.

## Enabling

Add to your board defconfig:

```
BR2_PACKAGE_KC110_BOARD_SUPPORT=y
# BR2_PACKAGE_GPIO_MOTORS is not set
```

`kc110-board-support` was developed against `hi3518ev200_ultimate_defconfig` but is hardware-specific to the KC110 — it should not be enabled on generic Hi3518EV200 boards.

## Build

The `ptz` binary is built reproducibly from `src/ptz.c` using the target toolchain. Same source → same `e70f78d73c91248ba044082e3f21c54b` md5 across rebuilds in our testing.

## Reference

KC110 reverse-engineering project that produced this package: <https://github.com/moontwister/kasa-kc110-openipc>.
