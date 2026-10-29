################################################################################
#
# xiongmai-opensdk-xm530v200
#
################################################################################

XIONGMAI_OPENSDK_XM530V200_SITE = $(call github,openipc,openxiongmai,$(XIONGMAI_OPENSDK_XM530V200_VERSION))
XIONGMAI_OPENSDK_XM530V200_VERSION = 5d769cc777c712a2ee48fbf4034c4d88f5fc3999

XIONGMAI_OPENSDK_XM530V200_LICENSE = MIT (libxmv200), GPL-2.0 (open_mmz, open_hevc, open_i2c, open_sys, open_vb, open_venc, open_rgn, open_vi, open_vpss kernel modules)
XIONGMAI_OPENSDK_XM530V200_LICENSE_FILES = libraries/libxmv200/LICENSE
XIONGMAI_OPENSDK_XM530V200_INSTALL_STAGING = YES

# The vendor modules it runs on, and load_xiongmai
XIONGMAI_OPENSDK_XM530V200_DEPENDENCIES = xiongmai-osdrv-xm530v200

# tools/xmv200-test builds libraries/libxmv200 first
define XIONGMAI_OPENSDK_XM530V200_BUILD_CMDS
	$(MAKE) CROSS_COMPILE=$(TARGET_CROSS) CFLAGS="$(TARGET_CFLAGS)" \
		-C $(@D)/tools/xmv200-test
endef

define XIONGMAI_OPENSDK_XM530V200_INSTALL_STAGING_CMDS
	$(INSTALL) -m 644 -D $(@D)/libraries/libxmv200/libxmv200.a $(STAGING_DIR)/usr/lib/libxmv200.a
	$(INSTALL) -m 644 -D $(@D)/libraries/libxmv200/xmv200.h $(STAGING_DIR)/usr/include/xmv200.h
endef

define XIONGMAI_OPENSDK_XM530V200_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -D $(@D)/tools/xmv200-test/xmv200-test $(TARGET_DIR)/usr/bin/xmv200-test
endef

# The open drivers, which load_xiongmai prefers over the vendor modules:
# drivers/xm_mmz (open_mmz.ko) with the V200 series' allocator,
# drivers/xm_hevc (open_hevc.ko) for this chip's engine (v3 on the
# XM530V200 and XM533, v1 on the XM535s), drivers/xm_v200_i2c
# (open_i2c.ko) for its I2C, drivers/xm_v200_sys (open_sys.ko) and
# drivers/xm_v200_vb (open_vb.ko) and drivers/xm_v200_venc (open_venc.ko)
# with its buffer layout, drivers/xm_v200_rgn (open_rgn.ko) for its OSD,
# on the XM533 drivers/xm533_vi (open_vi.ko), on the XM535D1/D2
# drivers/xm535_vpss (open_vpss.ko), on the XM535W1 drivers/xm535w1_vi
# (open_vi.ko). The kernel-module
# infrastructure installs them under /lib/modules/<kver>/extra/;
# xiongmai-osdrv-xm530v200 then leaves out the vendor mmz.ko, sys.ko,
# vb.ko, rgn.ko and those vi.ko and vpss.ko.
XIONGMAI_OPENSDK_XM530V200_MODULE_SUBDIRS = drivers/xm_mmz drivers/xm_hevc \
	drivers/xm_v200_i2c drivers/xm_v200_sys drivers/xm_v200_vb \
	drivers/xm_v200_venc drivers/xm_v200_rgn
ifeq ($(OPENIPC_SOC_MODEL),xm533)
XIONGMAI_OPENSDK_XM530V200_MODULE_SUBDIRS += drivers/xm533_vi
endif
ifneq ($(filter xm535d1 xm535d2,$(OPENIPC_SOC_MODEL)),)
XIONGMAI_OPENSDK_XM530V200_MODULE_SUBDIRS += drivers/xm535_vpss
endif
ifeq ($(OPENIPC_SOC_MODEL),xm535w1)
XIONGMAI_OPENSDK_XM530V200_MODULE_SUBDIRS += drivers/xm535w1_vi
endif
ifneq ($(filter xm530v200 xm533,$(OPENIPC_SOC_MODEL)),)
XIONGMAI_OPENSDK_XM530V200_HEVC_LAYOUT = v3
else
XIONGMAI_OPENSDK_XM530V200_HEVC_LAYOUT = v1
endif
ifneq ($(filter xm535%,$(OPENIPC_SOC_MODEL)),)
XIONGMAI_OPENSDK_XM530V200_I2C_CHIP = xm535
XIONGMAI_OPENSDK_XM530V200_VB_LAYOUT = xm535
else
XIONGMAI_OPENSDK_XM530V200_I2C_CHIP = $(OPENIPC_SOC_MODEL)
XIONGMAI_OPENSDK_XM530V200_VB_LAYOUT = v200
endif
ifeq ($(OPENIPC_SOC_MODEL),xm530v200)
XIONGMAI_OPENSDK_XM530V200_RGN_CHIP = v200
else ifeq ($(OPENIPC_SOC_MODEL),xm533)
XIONGMAI_OPENSDK_XM530V200_RGN_CHIP = xm533
else
XIONGMAI_OPENSDK_XM530V200_RGN_CHIP = xm535
endif
XIONGMAI_OPENSDK_XM530V200_MODULE_MAKE_OPTS = MMZ_ALLOCATOR=list \
	HEVC_LAYOUT=$(XIONGMAI_OPENSDK_XM530V200_HEVC_LAYOUT) \
	I2C_CHIP=$(XIONGMAI_OPENSDK_XM530V200_I2C_CHIP) \
	VB_LAYOUT=$(XIONGMAI_OPENSDK_XM530V200_VB_LAYOUT) \
	VENC_LAYOUT=$(XIONGMAI_OPENSDK_XM530V200_VB_LAYOUT) \
	RGN_CHIP=$(XIONGMAI_OPENSDK_XM530V200_RGN_CHIP)

$(eval $(kernel-module))
$(eval $(generic-package))
