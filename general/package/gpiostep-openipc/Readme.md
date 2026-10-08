## gpiostep-openipc

In-kernel GPIO half-step pan/tilt stepper driver + userspace CLI, for PTZ
cameras whose pan/tilt motors are 4-wire steppers wired straight to GPIO
(e.g. Goke **GK7205V510**, model NC-IPTC2200_DL).

This is the kernel-side counterpart to the userspace `gpio-motors` tool. Both
implement the same 8-phase half-step sequence and the same
`<pan> <tilt> <delay_ms>` command signature, so they can be compared 1:1. The
difference: `gpio-motors` writes `/sys/class/gpio` from userspace, whereas
`gpiostep` does `gpio_set_value()` directly in kernel context, which skips the
per-write syscall cost.

Timing depends on the kernel. The gk7205v500 kernel has high-resolution timers,
so every microstep delay is a precise sleep: a pan costs the CPU next to
nothing, and both axes run at the rate their delays ask for. On a kernel
without them any sleep rounds up to a whole 10ms tick, so delays under a quarter
tick busy-wait between scheduler yields (see `step_delay()` in
`src/gpiostep.c`) and longer ones sleep and accept the rounding. There a pan at
the GK7205V510's 2ms delay keeps the CPU busy for as long as it lasts, and the
sub-tick pacing only holds on an idle core.

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

### Speed and the acceleration ramp

A move from rest starts at `ramp_start_us` per micro-step and accelerates to
the delay it asked for over `ramp_microsteps` micro-steps; a delay of
`ramp_start_us` or longer runs flat. A move in the same direction that follows
the previous one within two micro-step periods carries on at its speed, so a
caller driving continuous motion as a train of short moves is not slowed to the
start rate by every one. The coils stay energised for `hold_ms` after a move,
which holds the rotor where the last micro-step left it, and are released after
that.

| parameter | default | |
|---|---|---|
| `ramp_start_us` | 2000 | delay a move from rest starts at; 0 turns the ramp off |
| `ramp_microsteps` | 64 | micro-steps to reach the requested delay |
| `hold_ms` | 20 | how long the coils stay on after a move |

All three are writable at runtime under `/sys/module/gpiostep/parameters/`.

Measured on a GK7205V510 pan/tilt head (40-step moves out and back, residual
position read from the picture):

| delay per micro-step | without the ramp | with the ramp |
|---|---|---|
| 833 us and slower | no steps lost | no steps lost |
| 767 us | no steps lost | — |
| 700 us | 2-22 steps lost per run | no steps lost |
| 650 us | — | no steps lost (256-micro-step ramp) |
| 600 us | — | steps lost |

So the ramp moves the limit from the start-up rate to the rate the motors can
run at, around 650 us here. 833 us (about 125 steps per second) leaves margin.
