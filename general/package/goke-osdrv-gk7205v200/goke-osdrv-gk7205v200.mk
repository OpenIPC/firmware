################################################################################
#
# goke-osdrv-gk7205v200
#
################################################################################

GOKE_OSDRV_GK7205V200_VERSION =
GOKE_OSDRV_GK7205V200_SITE =
GOKE_OSDRV_GK7205V200_LICENSE = MIT
GOKE_OSDRV_GK7205V200_LICENSE_FILES = LICENSE
GOKE_OSDRV_GK7205V200_INSTALL_STAGING = YES

define GOKE_OSDRV_GK7205V200_INSTALL_STAGING_CMDS
	$(INSTALL) -m 755 -d $(STAGING_DIR)/usr/include/goke
	#$(INSTALL) -m 644 -t $(STAGING_DIR)/usr/include/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/include/*

	## $(INSTALL) -m 755 -d $(BUILD_DIR)/goke-osdrv-gk7205v200/include
	## $(INSTALL) -m 644 -t $(BUILD_DIR)/goke-osdrv-gk7205v200/include $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/include/*

	## $(INSTALL) -m 755 -d $(BUILD_DIR)/goke-osdrv-gk7205v200/kmod
	## $(INSTALL) -m 644 -t $(BUILD_DIR)/goke-osdrv-gk7205v200/kmod $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/*.ko

	## $(INSTALL) -m 755 -d $(BUILD_DIR)/goke-osdrv-gk7205v200/lib
	## $(INSTALL) -m 644 -t $(BUILD_DIR)/goke-osdrv-gk7205v200/lib $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/*.so
endef

define GOKE_OSDRV_GK7205V200_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/script/S95goke

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/WDR
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/WDR $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/sensor/config/WDR/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.37/goke
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/cipher_drv.ko
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gfbg.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_acodec.ko
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_adc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_adec.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_aenc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_ai.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_aio.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_ao.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_base.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_chnl.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_h264e.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_h265e.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_isp.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_ive.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_jpege.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_rc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_rgn.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_sys.ko
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_tde.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_vedu.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_venc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_vgs.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_vi.ko
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_vo.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_vpss.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_wdt.ko
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/isp_piris.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/isp_pwm.ko
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/isp_sample_ist.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/isp_sensor_i2c.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/isp_sensor_spi.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/mipi_rx.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/osal.ko
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/ssp_ota5182_ex.ko
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/ssp_st7789_ex.ko
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/ssp_st7796_ex.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/sysconfig.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/script/load_*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/script/ircut_demo
	## $(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/script/set_allocator

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_aacdec.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_aacenc.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_aacsbrdec.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_aacsbrenc.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_ae.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_awb_natura.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_awb.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_bcd.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_cipher.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_isp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_ive.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_ivp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_md.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_mpi.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_qr.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_tde.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_vqe_aec.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_vqe_agc.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_vqe_anr.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_vqe_eq.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_vqe_hpf.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_vqe_record.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_vqe_res.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_vqe_talkv2.so
	##$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_vqe_wnr.so
endef

$(eval $(generic-package))
