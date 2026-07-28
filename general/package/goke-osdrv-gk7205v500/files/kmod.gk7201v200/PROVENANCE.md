# Provenance of the GK7201V200 (XiongMai xm72010200) vendor kernel modules

Everything in this directory is a **binary kernel module taken from the camera's own factory
firmware**. Nothing here was built from source, and no source exists in public. This file records
exactly where each file came from so the extraction can be repeated and audited independently.

## Why this set exists at all

`../kmod/` — the module set this package already shipped — is built **Apr  3 2023** and matches
the Apr-2023 `libxmedia_*.so` userspace in `../lib/`. It brings the GK7201V200 die up as far as
the ISP and then stops: `VEDU CreateChn` fails with `F008FFFF` and the VI->VPSS online link never
starts, so the camera produces no picture at all.

The set in this directory is built **Sep 23 2023** for chip id `xm72010200` and drives this exact
silicon correctly. Both sets carry the same `vermagic=4.9.37 ARMv7 thumb2 p2v8`, so they load on
the same kernel; they are two builds of the same vendor MPP, five months apart.

The Sep-2023 modules against the Apr-2023 userspace is the API skew that `gk7201-fixup`
(kernel module + `libgk_shim.so`) exists to bridge. See that package for the details.

Only the GK7201V200 build uses this directory. `gk7205v500`, `gk7205v510` and every other board
served by this package keep `../kmod/` untouched.

## Where they came from

  Device        XiongMai GK7201V200 + SC2336 IP camera, factory firmware, as sold
  Method        full 16 MiB NOR dump read off the device with `defib`, 2026-07-13
  Dump file     xm720_full_16mb.bin, 16777216 bytes,
                md5 d87d463bcc7ed9a6f9f11be2814ee914
  Path inside   /mnt/mtd/ipc/modules/          (the vendor's own module directory)
  extdrv/       /mnt/mtd/ipc/modules/extdrv/   (sinit.ko, the sensor-init helper)

The vendor's own loader in that image is `/mnt/mtd/ipc/modules/load`, invoked by its
`load_media.sh` as:

    cd /mnt/mtd/ipc/modules
    ./load xm72010200 -i -sensor0 sc2336

`files/script/load_xm` in this package is derived from that sequence, module for module, with the
MMZ layout adjusted to OpenIPC's `mem=32M`. The vendor original is not reproduced here — it is
readable in any dump of the same firmware.

To repeat the extraction on your own camera: dump the flash, `binwalk`/`unsquashfs` the `mtd`
partition holding `/mnt/mtd`, and copy `ipc/modules/`. The md5s below let you check that you got
the same build.

## Licensing, stated honestly

Every module declares `license=GPL` in its `.modinfo`, yet XiongMai/Goke publish no corresponding
source. That is the vendor's problem, not something this port can fix, and it is exactly the same
situation as the stock `../kmod/` set that this package has always carried — these files are
redistributed on the same footing. They are **not stripped** (xm_vpss.ko alone exports 823
symbols), which is what made this port possible: every offset used by `gk7201_fixup` was read out
of the vendor's own symbol table rather than guessed.

If a maintainer prefers not to carry a second binary set in-tree, the alternative is a build-time
fetch or an extraction script pointed at the user's own firmware dump — say so and it can be
reshaped that way. The files are listed individually below precisely so that decision is informed.

## Contents (md5)

  e1a2a3fb9d537c88ec6d7311ca8a60bb  extdrv/sinit.ko
  feac6cb914bfd1d8df9709bdea7ddd63  xm_acodec.ko
  37666fa272bc3a4baf483d3b6ca982ca  xm_adc.ko
  a9f78e24a6cc87f36d335520669d87b1  xm_adec.ko
  517702d7d9ae4fe97b3b5d31fa3c691c  xm_aenc.ko
  ac8386499bd21739ce085a1bec9b4a68  xm_ai.ko
  1045f23611e79ec58f0c866ff7bb1cc5  xm_aio.ko
  715017069dbf63d37f49a653492f2541  xm_ao.ko
  4cb86e43118090f9d4ac463cbd066c60  xm_base.ko
  cd1df88a9a71d9937d1b87ce67c99ed4  xm_chnl.ko
  7a674e79d9cb01fccecf9e7c5dd1fe89  xm_cipher_drv.ko
  bfa47f56893a15aae8c67488282ca649  xm_gfbg.ko
  bc68064916bcac0e20897c563e173896  xm_h264e.ko
  5fb3c3c8ea9a721b26b02e17015205f4  xm_h265e.ko
  3dca60d88f61653ca98fcff9328d1237  xm_init.ko
  1b0f0e8d67fd153dc9d389d5d56288c1  xm_isp.ko
  6b9aef1c1363f21b3706a1c4bef9f7f6  xm_isp_piris.ko
  e67d2198c5f540e5062da989e7774d47  xm_isp_pwm.ko
  3253c1fd2128c9c7c32635b2df8bcb89  xm_isp_sample_ist.ko
  9c5283da55e9f9272dc3b970229c390a  xm_isp_sensor_i2c.ko
  3d90261bc6adbb0f93f987f4d25c16ac  xm_isp_sensor_spi.ko
  69d0cd967628e3cbba1199e19ed1474b  xm_ive.ko
  2b64b5fc4e163634fea52877cd402aeb  xm_jpege.ko
  8387293fa2504d7a70e0f21714011769  xm_mipi_rx.ko
  7e284c7c3b0d68d743eaf9c63c8c30dd  xm_npu.ko
  fa77df762f45bfd438be5cf5ccccd3c3  xm_osal.ko
  c29151d7acd50fcba3ae8fecdad805b6  xm_pdm.ko
  9b49c31582a8fd0c3e1dd2d5368cd2d4  xm_pm.ko
  1cafb2414b1c88b3e7528ce6776e0750  xm_rc.ko
  3f60be76582749848d17cdc6911f97af  xm_rgn.ko
  553c90abd417878668986942c0499e5e  xm_ssp_ota5182_ex.ko
  d7efeb077de8acce04ae5a4b6f868702  xm_ssp_st7789_ex.ko
  98b240583dd87e617fed6e5fe3128bae  xm_ssp_st7796_ex.ko
  bf6324580f3132fc3294bd225626958a  xm_sysconfig.ko
  bdd5c15e742721c294e7322c7bd59f1d  xm_sys.ko
  746839098f9889b3d5a36c090a935b46  xm_tde.ko
  591b6b049d3e0ae408bf3307fafbf086  xm_vedu.ko
  b538fb0d7b9104e59dc22b74fa48c552  xm_venc.ko
  c2a7b678526cf4c299440e0761326872  xm_vgs.ko
  b0b821cded1dcaee8334d84b05233d15  xm_vi.ko
  ca55fc82401282eac8458918bfdf9276  xm_vo.ko
  3b5b0e790d00e6f2d398255236032320  xm_vpss.ko
  cfc71d0c768af5ca76c65320daf51ffc  xm_wdt.ko

  42 modules + extdrv/sinit.ko, 2.2 MiB total.
