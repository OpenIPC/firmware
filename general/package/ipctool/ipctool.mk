################################################################################
#
# ipctool
#
################################################################################

IPCTOOL_SITE = $(call github,openipc,ipctool,$(IPCTOOL_VERSION))
IPCTOOL_VERSION = HEAD

IPCTOOL_LICENSE = MIT
IPCTOOL_LICENSE_FILES = LICENSE
IPCTOOL_INSTALL_STAGING = YES

IPCTOOL_CONF_OPTS += -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release -DSKIP_VERSION=ON
IPCTOOL_MAKE_OPTS += VERBOSE=1

# ipcinfo links libipchw, which knows every SoC vendor and every HiSilicon
# generation ipctool has ever learned: a detection table for eleven vendor
# HALs, and a chip-ID table, a sensor-bus back-end, a temperature formula and
# a die-ID reader for each of ten HiSilicon generations. A camera is one SoC.
# The rest is code that cannot execute on it, and it ships on all but three of
# this tree's board configs, onto boards whose rootfs cap leaves tens of KB
# spare.
#
# Upstream takes both as ;-separated build options defaulting to "all"
# (ipctool #178 and #211). majestic already derives them the same way, from
# its VENDOR and SDK code -- see majestic's CMakeLists.txt. Nothing here
# derives IPCHW_SENSORS: a mainline image is per-family and gets flashed onto
# whatever camera someone bought, and ipcinfo --short-sensor exists to name a
# sensor nobody catalogued. Compiling a probe family out turns that into
# "SENSOR is not detected, aborting". IPCHW_PADMUX is left alone too -- gc
# already drops it from ipcinfo, and libipchw carries its selection as a
# PUBLIC compile definition, which would break config_tool in
# sigmastar-osdrv-infinity6.
#
# An unmapped vendor or family passes nothing and keeps upstream's default, so
# a new SoC is never silently trimmed to the wrong thing -- it just does not
# benefit until someone maps it.

# Which vendor HAL a board can reach. HiSilicon's is not optional upstream and
# Goke is a HiSilicon derivative detected through the same path, so both ask
# for none of the eleven.
IPCTOOL_VENDORS_allwinner  = allwinner
IPCTOOL_VENDORS_fullhan    = fh
IPCTOOL_VENDORS_goke       = none
IPCTOOL_VENDORS_grainmedia = gm
IPCTOOL_VENDORS_hisilicon  = none
IPCTOOL_VENDORS_ingenic    = ingenic
IPCTOOL_VENDORS_novatek    = novatek
IPCTOOL_VENDORS_rockchip   = rockchip
IPCTOOL_VENDORS_sigmastar  = sstar
IPCTOOL_VENDORS_xiongmai   = xm

# Which HiSilicon generation, by family. These names are not a coincidence:
# ipctool's getchipfamily() returns the same strings, so `ipcinfo -f` on a
# board prints the key this table is indexed by.
IPCTOOL_HISI_gk7205v200  = v4
IPCTOOL_HISI_gk7205v500  = v4
IPCTOOL_HISI_hi3516av100 = v2a
IPCTOOL_HISI_hi3516cv100 = v1
IPCTOOL_HISI_hi3516cv200 = v2
IPCTOOL_HISI_hi3516cv300 = v3
IPCTOOL_HISI_hi3516cv500 = v4a
IPCTOOL_HISI_hi3516cv6xx = v5
IPCTOOL_HISI_hi3516ev200 = v4
IPCTOOL_HISI_hi3519dv500 = v5
IPCTOOL_HISI_hi3519v101  = v3a
IPCTOOL_HISI_hi3536cv100 = 3536c
IPCTOOL_HISI_hi3536dv100 = 3536d

# A vendor that is not HiSilicon reaches hal_hisi through nothing: the only
# route is a HiSilicon UART0 base in chipid.c's dispatch. Dropping it takes
# hal_hisi.c and ispreg.c out of the library altogether.
#
# hi3520dv200 and gk710x are deliberately absent from the table above. The
# 3520DV200 ID sets no chip_generation at all, and no ID in ipctool's table
# matches a GK7101/GK7102, so neither has a generation to name -- they keep
# every one rather than be trimmed on a guess.
IPCTOOL_HISI_VENDOR_allwinner  = none
IPCTOOL_HISI_VENDOR_fullhan    = none
IPCTOOL_HISI_VENDOR_grainmedia = none
IPCTOOL_HISI_VENDOR_ingenic    = none
IPCTOOL_HISI_VENDOR_novatek    = none
IPCTOOL_HISI_VENDOR_rockchip   = none
IPCTOOL_HISI_VENDOR_sigmastar  = none
IPCTOOL_HISI_VENDOR_xiongmai   = none

IPCTOOL_IPCHW_VENDORS = $(IPCTOOL_VENDORS_$(OPENIPC_SOC_VENDOR))
IPCTOOL_IPCHW_HISI = $(or $(IPCTOOL_HISI_$(OPENIPC_SOC_FAMILY)),\
	$(IPCTOOL_HISI_VENDOR_$(OPENIPC_SOC_VENDOR)))

ifneq ($(IPCTOOL_IPCHW_VENDORS),)
IPCTOOL_CONF_OPTS += -DIPCHW_VENDORS=$(IPCTOOL_IPCHW_VENDORS)
endif
ifneq ($(IPCTOOL_IPCHW_HISI),)
IPCTOOL_CONF_OPTS += -DIPCHW_HISI=$(IPCTOOL_IPCHW_HISI)
endif

define IPCTOOL_INSTALL_STAGING_CMDS
	$(INSTALL) -m 755 -t $(STAGING_DIR)/usr/lib $(@D)/libipchw.a
endef

define IPCTOOL_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/ipcinfo
endef

$(eval $(cmake-package))
