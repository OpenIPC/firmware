################################################################################
#
# xiongmai-opensdk-xm530v200
#
################################################################################

XIONGMAI_OPENSDK_XM530V200_SITE = $(call github,openipc,openxiongmai,$(XIONGMAI_OPENSDK_XM530V200_VERSION))
XIONGMAI_OPENSDK_XM530V200_VERSION = fea86026c7a2f25f7acf66b83514f1c3ff274018

XIONGMAI_OPENSDK_XM530V200_LICENSE = MIT (libxmv200), GPL-2.0 (open_mmz, open_hevc, open_i2c, open_sys, open_vb, open_venc, open_rgn, open_vi, open_vpss, open_acodec, open_i2s, open_adc, open_ddr, open_log, open_mipi_io, open_h264, open_jpeg, open_venc_mem, open_nn, open_ive, open_vo, open_isp, open_xm_usb0, open_rtc, open_vdec, open_var, open_dma, open_motor kernel modules), GPL-2.0+ or BSD-3-Clause (open_dwc2 kernel module)
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
# (open_vi.ko), on the XM530V200 drivers/xm530v200_vpss (open_vpss.ko).
# The small modules (open_acodec, open_i2s, open_adc, open_ddr, open_log,
# open_nn, the XM533's open_mipi_io, open_venc_mem and open_ive, the
# XM533's and XM535s' open_h264 (drivers/hx280enc, H264_CHIP=v200), the
# XM530V200's open_jpeg, the XM535s' open_isp) replace vendor
# modules nothing links against,
# which stay as the fallback. On the XM530V200 drivers/xm530v200_vo
# (open_vo.ko) replaces vo.ko, which the VPSS links against; on the XM533
# drivers/xm533_isp (open_isp.ko) replaces isp.ko, which vi.ko links
# against, and on the XM530V200 drivers/xm530v200_isp, which vpss.ko links
# against. On the XM530V200 and XM535s drivers/dwc2 (open_dwc2.ko) and
# drivers/xm_v200_usb0 (open_xm_usb0.ko) are the USB host, in place of
# dwc2.ko and xm_usb0.ko. On the XM530V200 drivers/xm_v200_rtc (open_rtc),
# xm_v200_vdec (open_vdec), xm_v200_var (open_var) and xm_v200_dma
# (open_dma) stand in for rtc.ko, vdec.ko, var.ko and xm_dma.ko, which the
# image does not carry, and on every chip drivers/xm_v200_motor (open_motor)
# for kmotor.ko (the XM535D2's through a shift register, MOTOR_CHIP=xm535d2).
# The kernel-module infrastructure installs them under /lib/modules/<kver>/extra/;
# xiongmai-osdrv-xm530v200 then leaves out the vendor mmz.ko, sys.ko,
# vb.ko, rgn.ko and those vi.ko, vpss.ko, vo.ko and isp.ko.
XIONGMAI_OPENSDK_XM530V200_MODULE_SUBDIRS = drivers/xm_mmz drivers/xm_hevc \
	drivers/xm_v200_i2c drivers/xm_v200_sys drivers/xm_v200_vb \
	drivers/xm_v200_venc drivers/xm_v200_rgn drivers/xm_v200_acodec \
	drivers/xm_v200_i2s drivers/xm_v200_adc drivers/xm_v200_ddr drivers/xm_v200_log \
	drivers/xm_v200_nn drivers/xm_v200_motor
ifeq ($(OPENIPC_SOC_MODEL),xm533)
XIONGMAI_OPENSDK_XM530V200_MODULE_SUBDIRS += drivers/xm533_vi drivers/xm533_mipi_io \
	drivers/xm533_venc_mem drivers/xm533_ive drivers/xm533_isp
endif
ifneq ($(filter xm533 xm535%,$(OPENIPC_SOC_MODEL)),)
XIONGMAI_OPENSDK_XM530V200_MODULE_SUBDIRS += drivers/hx280enc
endif
ifneq ($(filter xm535d1 xm535d2,$(OPENIPC_SOC_MODEL)),)
XIONGMAI_OPENSDK_XM530V200_MODULE_SUBDIRS += drivers/xm535_vpss drivers/xm535_isp
endif
ifeq ($(OPENIPC_SOC_MODEL),xm535w1)
XIONGMAI_OPENSDK_XM530V200_MODULE_SUBDIRS += drivers/xm535w1_vi drivers/xm535w1_isp
endif
ifeq ($(OPENIPC_SOC_MODEL),xm530v200)
XIONGMAI_OPENSDK_XM530V200_MODULE_SUBDIRS += drivers/xm530v200_vpss drivers/xm530v200_jpeg \
	drivers/xm530v200_vo drivers/xm530v200_isp drivers/xm_v200_rtc \
	drivers/xm_v200_vdec drivers/xm_v200_var drivers/xm_v200_dma
endif
ifneq ($(filter xm530v200 xm535%,$(OPENIPC_SOC_MODEL)),)
XIONGMAI_OPENSDK_XM530V200_MODULE_SUBDIRS += drivers/dwc2 drivers/xm_v200_usb0
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
# the small modules' chip variants
ifeq ($(OPENIPC_SOC_MODEL),xm530v200)
XIONGMAI_OPENSDK_XM530V200_SMALL = ACODEC_CHIP=v200 I2S_CHIP=v200 ADC_CHIP=v200 DDR_CHIP=v200 NN_CHIP=v200
else ifeq ($(OPENIPC_SOC_MODEL),xm533)
XIONGMAI_OPENSDK_XM530V200_SMALL = ACODEC_CHIP=xm533 I2S_CHIP=v200 ADC_CHIP=v200 NN_CHIP=xm533
else ifeq ($(OPENIPC_SOC_MODEL),xm535w1)
XIONGMAI_OPENSDK_XM530V200_SMALL = ACODEC_CHIP=xm535w1 I2S_CHIP=xm535w1 ADC_CHIP=xm535 NN_CHIP=xm535 USB0_CHIP=xm535
else
XIONGMAI_OPENSDK_XM530V200_SMALL = ACODEC_CHIP=xm535 I2S_CHIP=xm535 ADC_CHIP=xm535 NN_CHIP=xm535 USB0_CHIP=xm535
endif
ifeq ($(OPENIPC_SOC_MODEL),xm535d2)
XIONGMAI_OPENSDK_XM530V200_MOTOR_CHIP = xm535d2
endif
XIONGMAI_OPENSDK_XM530V200_MODULE_MAKE_OPTS = MMZ_ALLOCATOR=list \
	MOTOR_CHIP=$(XIONGMAI_OPENSDK_XM530V200_MOTOR_CHIP) \
	HEVC_LAYOUT=$(XIONGMAI_OPENSDK_XM530V200_HEVC_LAYOUT) \
	I2C_CHIP=$(XIONGMAI_OPENSDK_XM530V200_I2C_CHIP) \
	VB_LAYOUT=$(XIONGMAI_OPENSDK_XM530V200_VB_LAYOUT) \
	VENC_LAYOUT=$(XIONGMAI_OPENSDK_XM530V200_VB_LAYOUT) \
	RGN_CHIP=$(XIONGMAI_OPENSDK_XM530V200_RGN_CHIP) H264_CHIP=v200 \
	$(XIONGMAI_OPENSDK_XM530V200_SMALL)

$(eval $(kernel-module))
$(eval $(generic-package))
