################################################################################
#
# hisilicon-osdrv-hi3516cv6xx
#
################################################################################

HISILICON_OSDRV_HI3516CV6XX_VERSION =
HISILICON_OSDRV_HI3516CV6XX_SITE =
HISILICON_OSDRV_HI3516CV6XX_LICENSE = MIT
HISILICON_OSDRV_HI3516CV6XX_LICENSE_FILES = LICENSE

define HISILICON_OSDRV_HI3516CV6XX_INSTALL_TARGET_CMDS

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/5.10.221/hisilicon
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/extdrv/ot_sensor_i2c.ko

	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_acodec.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_adec.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_aenc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_ai.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_aio.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_ao.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_base.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_chnl.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_cipher.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_h264e.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_h265e.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_isp.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_ive.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_jpege.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_km.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_mipi_rx.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_mmz.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_osal.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_pm.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_rc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_rgn.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_spi_dma_transfer.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_svp_npu.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_sys.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_vb.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_vca.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_venc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_vgs.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_vi.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_vpp.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_vpss.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/ot_wdt.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/5.10.221/hisilicon $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/kmod/sys_config.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/script/load*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib


endef

$(eval $(generic-package))
