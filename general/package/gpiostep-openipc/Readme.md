## gpiostep-openipc

In-kernel GPIO half-step pan/tilt stepper driver + userspace CLI, for PTZ
cameras whose pan/tilt motors are 4-wire steppers wired straight to GPIO
(e.g. Goke **GK7205V510**, model NC-IPTC2200_DL).

This is the kernel-side counterpart to the userspace `gpio-motors` tool. Both
implement the same 8-phase half-step sequence and the same
`<pan> <tilt> <delay_ms>` command signature, so they can be compared 1:1. The
difference: `gpio-motors` toggles `/sys/class/gpio` (open/write/close per pin
per microstep) from userspace, whereas `gpiostep` does `gpio_set_value()`
directly in kernel context — steadier timing and far lower CPU.

### Load

```
insmod /lib/modules/$(uname -r)/extra/gpiostep.ko          # GK7205V510 defaults
# or override the pin map:
insmod gpiostep.ko pan_gpios=3,4,72,73 tilt_gpios=69,59,58,57
```

The module creates `/dev/motorDev`.

### Use

```
gpiostep-ctl <pan steps> <tilt steps> <delay (ms)>

gpiostep-ctl 20 0 30     # pan +20 steps
gpiostep-ctl 0 -20 30    # tilt -20 steps
gpiostep-ctl 20 10 30    # diagonal
```

A "step" is one full 8-phase cycle, matching `gpio-motors`. The default pin map
(`pan = 3,4,72,73`, `tilt = 69,59,58,57`) was derived from the vendor
`/proc/devcfg` motor block on a GK7205V510. If a coil phase is reversed or
motion is rough, try the vendor coil order `[0,2,1,3]`:
`pan_gpios=3,72,4,73 tilt_gpios=69,58,59,57`.
