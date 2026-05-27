# kc110-mipi-shim

Buildroot package that builds `libfixmipi.so` — a small `LD_PRELOAD` shim that patches `majestic`'s MIPI ioctl calls at runtime to work around an upstream bug in how `combo_dev_attr_t` and the VIU `VI_DEV_ATTR_S` are constructed for MIPI sensors on the Hi3518EV200 (V2A) family.

## Status: workaround, not a real fix

The right fix is in `majestic` itself — it currently builds `combo_dev_attr_t` with `raw_data_type=0` (uninitialized) and `VI_DEV_ATTR_S.enIntfMode=DC` for MIPI sensors, both of which `hi_mipi.ko` rejects. Without something correcting the values in flight, majestic boots, reaches `MPI_VI_SetMipiAttr`, and fails:

```
ioctl HI_MIPI_SET_DEV_ATTR failed
Cannot set VI Mipi attr
Cannot init sensor
Cannot start SDK
```

`/proc/driver/hi_mipi` then reports `LaneNum=4, DataType=N/A, PhyMode=CMOS` — the uninitialized defaults — instead of the correct `LaneNum=2, RAW10, MIPI` for OV2735.

The shim hooks `ioctl()` and patches the two relevant calls in flight:

| ioctl | Patch |
|---|---|
| `HI_MIPI_SET_DEV_ATTR` (0x41306d01) | `raw_data_type=2` (RAW10), `lane_id=[0, 1, -1, -1, -1, -1, -1, -1]` |
| `VIU_DEV_ATTR_SET` | `enIntfMode=VI_MODE_MIPI=5`, `au32CompMask[0]=0xfff00000` |

After the shim, `/proc/driver/hi_mipi` shows the correct values and majestic's pipeline comes up.

## Activation

The package installs `/usr/lib/libfixmipi.so`. Activation requires `LD_PRELOAD=/usr/lib/libfixmipi.so` in the majestic init script (typically `/etc/init.d/S95majestic`). The package intentionally does not modify that init script — users adopt the package by either patching their own S95majestic or maintaining a board-specific overlay that does the export.

## Hardware tested

- TP-Link Kasa KC110 (HiSilicon Hi3518EV200 + OV2735 MIPI sensor)

Live-verified: without the shim, majestic fails at `HI_MIPI_SET_DEV_ATTR` and no frames flow. With the shim, RTSP + JPEG come up, sensor i2c writes land (post-`openhisilicon#197`), AE converges.

## Why upstream

This is hardware-specific enough to be useful for other Hi3518EV200 boards running an MIPI sensor through OpenIPC's majestic. Each board owner that hits the same `HI_MIPI_SET_DEV_ATTR` failure currently has to either rediscover the same shim or hand-patch an alternative — making it a package means the workaround is reviewable, versioned, and easy to remove the day majestic stops needing it.

## Removal plan

Once `majestic` builds `combo_dev_attr_t` and `VI_DEV_ATTR_S` correctly for MIPI sensors:

1. Drop `BR2_PACKAGE_KC110_MIPI_SHIM=y` from any defconfig that enables it.
2. Remove the `LD_PRELOAD` export from the init script.
3. Verify majestic starts cleanly without the shim.

The package can then be deleted entirely.
