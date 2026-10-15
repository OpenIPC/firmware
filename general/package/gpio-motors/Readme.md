## Introduction

This package is created and intended for unification and use in all firmware and on various processors where the method of connecting motors via GPIO is used.


## Configuring known devices

#### Basic Example
```
fw_setenv ptz_gpio 'H1 H2 H3 H4 V1 V2 V3 V4'
```
`ptz_gpio` is the current name for the pin list; `gpio_motors`, used in the
examples below and on cameras configured before the rename, keeps working as
a fallback.
First, the GPIOs responsible for Horizontal rotation are added, and next then the GPIOs responsible for Vertical rotation are added, a total of 8 characters.

#### GK7205V200 (unknown model)
```
fw_setenv gpio_motors '52 53 56 57 69 70 59 58'
```

#### GK7205V510, NC-IPTC2200_DL (Goke gk7205v500 family)
```
fw_setenv gpio_motors '3 4 72 73 69 59 58 57'
```
Pan (roll) on GPIOs `3 4 72 73`, tilt (pitch) on GPIOs `69 59 58 57`, derived from the
vendor `/proc/devcfg` motor map. If motion is rough or a phase is reversed, try the vendor
coil order `[0,2,1,3]`: `fw_setenv gpio_motors '3 72 4 73 69 58 59 57'`.

#### GM8136, Faleemi
```
fw_setenv gpio_motors '51 52 53 54 55 28 29 30'
```

#### HI3516CV200, FDT
```
fw_setenv gpio_motors '60 61 37 38 54 55 56 57'
```

#### SSC337DE, Foscam
```
fw_setenv gpio_motors '1 2 12 13 62 63 64 65'
```

#### SSC30KD, CMCC DS-YTJ5101
```
fw_setenv gpio_motors '111 112 113 114 59 60 8 9'
```

#### T31, Jooan Q3R-U
```
fw_setenv gpio_motors 53 52 54 14 17
```
This type of device has only 4 GPIOs intended for controlling motors at one time and 1 additional GPIO for switching the Horizontal/Vertical mode. 
It is on this device that temporary incorrect operation of the camera's diagonal movement is possible.

## Using the utility
```
gpio-motors <pan steps> <tilt steps> <delay (ms)>
gpio-motors status
```

```
gpio-motors 20 0 30
gpio-motors -20 0 30
#
gpio-motors 0 20 30
gpio-motors 0 -20 30
#
gpio-motors 20 10 30
gpio-motors -20 10 30
```

## End switches (optional)

Heads with end switches can stop on them instead of running into the
mechanical stop:
```
fw_setenv ptz_limits '<pan+> <pan-> <tilt+> <tilt-> [active]'
```
Each value is the GPIO of the switch that a positive or negative move on that
axis runs into, or `-1` if there is none. Switches are active-low by default.
Add a fifth value of `1` for active-high. Without `ptz_limits` nothing changes.

- The switch for the direction being moved is read before every micro-step.
  When it closes, the axis stops and prints `end stop reached, N steps not done`.
- The axis then backs off until the switch releases, plus 3 steps of margin.
  The back-off is capped at 40 steps, so a switch that never releases cannot
  run the head away. It prints a warning instead. The back-off also stops if
  the switch at the other end closes.
- A move away from a pressed switch always runs.
- A switch that cannot be read counts as not pressed.
- A pin that is also in `ptz_gpio`, or that is listed for both ends, disables
  all limits with a message. One shared switch cannot tell which end is
  pressed. So does any value that is not a whole number, a pin below `-1`, a
  polarity other than `0` or `1`, or more than five values.
- A switch pin that something else already exported as an output (an IR-cut
  pin, for example) is not touched and counts as unreadable.

`gpio-motors status` prints each switch and whether it is pressed. Use it to
check the wiring and polarity before moving the head.

## More info

- [Our Wiki](https://github.com/OpenIPC/wiki/blob/master/en/motors.md)
- [Motors repo](https://github.com/openipc/motors)
