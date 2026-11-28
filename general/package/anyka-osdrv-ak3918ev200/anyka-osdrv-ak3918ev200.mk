################################################################################
#
# anyka-osdrv-ak3918ev200
#
################################################################################

ANYKA_OSDRV_AK3918EV200_VERSION =
ANYKA_OSDRV_AK3918EV200_SITE =
ANYKA_OSDRV_AK3918EV200_LICENSE = PROPRIETARY

# files/sensor/<sensor>.conf is the ISP tuning of one sensor, named as
# ipctool names the sensor (`ipcinfo -s`), not always as the vendor did.
# The kernel's ISP driver (drivers/misc/ak_isp_tuning.c) reads
# /etc/sensor/<name>.conf for the
# sensor it detected when capture first starts, and only that one.
#
# A tuning is kept whenever a stock firmware yields one, driver or not:
# tunings are the scarce part, found only inside camera images, so the
# one for a sensor the kernel cannot drive yet is here when it can. A
# file without a driver is never opened: today mis2006 and sc1346. gc1054
# and sc2335 run over DVP since OpenIPC/linux#292; sc2335's 1280x960 does
# not fit the 24 MiB video pool yet.
#
# Every file is a vendor tuning from the stock firmware of an EV200-class
# camera, converted from the vendor's "3.0xx" layout to the "3.05_032401"
# one the kernel validates with files/sensor/convert-3.004.py. The
# conversion changes header fields only (version string, style count and
# style size) and gives each style style 0's sensor register list, the
# only one the kernel writes; no tuning value is changed.
#
#   gc1004.conf  isp_gc1024.conf      v3.000      AK3918EV200 camera
#   gc1034.conf  isp_gc1034_0.conf    v3.001      V380E2 camera
#   gc1054.conf  isp_gc1054/base.conf yl-0322     AK3918EV201 camera (*)
#   jxh62.conf   isp_h62.conf         v3.004      AnyCloud39EV300 SDK
#                (UVC_check_sensor; a webcam tuning, the only H62 one)
#   mis2006.conf isp_mis2006/base.conf 1.005_0919 AK3918EV201 camera (*)
#   sc1135.conf  isp_sc1135_0.conf    v3.015      V380E2 camera
#   sc1145.conf  isp_sc1145.conf      v3.006      AK3918EV200 camera
#   sc1235.conf  isp_sc1235.conf      v3.012      AK3918EV200 camera
#   sc1345.conf  isp_sc1345_dvp.conf  v3.004      Gwell camera
#   sc1346.conf  isp_sc1346/base.conf DVP_V3_0411 AK3918EV201 camera (*)
#   sc2145h.conf isp_sc1245.conf      v3.011      AK3918EV200 camera
#   sc2235p.conf isp_sc2232.conf      v3.000      V380E2 camera
#   sc2335.conf  isp_sc2335/base.conf 2021.7.07   AK3918EV201 camera (*)
#
# (*) An AliOS Things camera (Macro Video app). Its romfs keeps every
# tuning LZMA-packed: a 28-byte header (chunk size 0x10000 first, the
# unpacked size third) and one LZMA-alone stream per 64 KiB after it.
# Unpacked, each is the EV200 layout above with 3 styles, and its version
# field holds a label instead of "3.0xx". Its sc1345 tuning is not taken
# (the Gwell one above is what the lab boards stream with), nor its two
# board-specific sc1346 variants (isp_sc1346/c1/l0p0.conf, l0p1.conf).
#
# Where a camera carried several, the newest generic one was taken. The
# EV300 and EV330 tunings ("4.x", "5.x") are laid out for other ISP
# libraries and are not usable here.

define ANYKA_OSDRV_AK3918EV200_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensor
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensor \
		$(ANYKA_OSDRV_AK3918EV200_PKGDIR)/files/sensor/*.conf

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin \
		$(ANYKA_OSDRV_AK3918EV200_PKGDIR)/files/script/load_anyka
endef

$(eval $(generic-package))
