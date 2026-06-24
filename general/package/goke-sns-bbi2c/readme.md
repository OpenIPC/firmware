# goke-sns-bbi2c

Software (GPIO bit-bang) i2c sensor bring-up for **Goke gk7205v200** boards whose
image sensor is wired to GPIO pins instead of the SoC hardware i2c controller.

## Why
On some gk7205v200/v210 cameras the image sensor's i2c is **not** on any of the
SoC hardware i2c controllers (`0x12060000/0x12061000/0x12062000`) — it is bit-banged
over GPIO. As a result OpenIPC's hardware-i2c sensor probe never gets an ACK and the
sensor appears "not detected" / `I2C_WRITE error`. This is the failure seen in
OpenIPC/firmware issues **#1930** (gk7205v200 + os02g10) and **#1404**
(gk7205v210 + sc223a).

Reverse-engineering one such camera — **CP Plus CP-UNC-TA21L2C** (Dahua-OEM,
SmartSens **SC223A**, chip-id `0xcb3e`) — showed the sensor is on a software i2c bus:

| signal | pin | pad |
|---|---|---|
| SDA | GPIO7_2 | `0x112c0060` |
| SCL | GPIO7_3 | `0x112c0064` |
| sensor reset / enable | GPIO8_7 (drive high) | `0x112c0048` |
| MCLK 27 MHz | CRG `0x120100f0 = 0x19` | — |
| MIPI-RX ref clock | CRG `0x120100f4 = 0x10340` | — |

## What it does
`goke_sns_bbi2c.ko` registers a real Linux i2c adapter (`/dev/i2c-N`) that bit-bangs
SDA=GPIO7_2 / SCL=GPIO7_3, drives GPIO8_7 high to release the sensor from reset, sets
the 27 MHz MCLK, and holds the MIPI-RX reference clock stable (it re-asserts MCLK and
pads on every transfer but never disturbs `f4`, so majestic's auto-exposure writes can't
corrupt the streaming clock — that was the cause of the MIPI PHY never locking).

The init script `S96sensor-bbi2c` loads the module, wakes the sensor, runs
`load_goke -a -sensor sc223a -board demo`, remaps `/dev/i2c-0` to the bit-bang bus
(majestic's libsns opens `/dev/i2c-0`), and starts majestic. It replaces the generic
majestic autostart for this board.

## Result
Verified on hardware: MIPI PHY locks at 1920x1080 with zero CRC/ECC errors, and majestic
produces correct color 1080p H.264 / MJPEG with the existing `libsns_sc223a.so`.

## Notes / scope
- Targets gk7205v200; GPIO/CRG addresses are SoC-specific.
- The sensor (SC223A) is already supported by OpenIPC's `libsns_sc223a.so`; this package
  only adds the missing **board** glue (bit-bang i2c + sensor enable + MIPI clock).
- A device-tree `i2c-gpio` adapter would be the more idiomatic long-term form; this 4.9
  vendor kernel has no DT for these pins and `CONFIG_I2C_GPIO` is not enabled, so a small
  self-registering module is used instead.
