################################################################################
#
# goke-osdrv-gk7205v200
#
################################################################################

GOKE_OSDRV_HI3516EV300_VERSION =
GOKE_OSDRV_HI3516EV300_SITE =
GOKE_OSDRV_HI3516EV300_LICENSE = MIT
GOKE_OSDRV_HI3516EV300_LICENSE_FILES = LICENSE
GOKE_OSDRV_HI3516EV300_INSTALL_STAGING = YES


define GOKE_OSDRV_HI3516EV300_INSTALL_STAGING_CMDS
	$(INSTALL) -m 755 -d $(STAGING_DIR)/usr/include/goke
	#$(INSTALL) -m 644 -t $(STAGING_DIR)/usr/include/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/include/*

	## $(INSTALL) -m 755 -d $(BUILD_DIR)/goke-osdrv-gk7205v200/include
	## $(INSTALL) -m 644 -t $(BUILD_DIR)/goke-osdrv-gk7205v200/include $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/include/*

	## $(INSTALL) -m 755 -d $(BUILD_DIR)/goke-osdrv-gk7205v200/kmod
	## $(INSTALL) -m 644 -t $(BUILD_DIR)/goke-osdrv-gk7205v200/kmod $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/*.ko

	## $(INSTALL) -m 755 -d $(BUILD_DIR)/goke-osdrv-gk7205v200/lib
	## $(INSTALL) -m 644 -t $(BUILD_DIR)/goke-osdrv-gk7205v200/lib $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/*.so
endef


define GOKE_OSDRV_HI3516EV300_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	#$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/script/S99goke

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/WDR
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/WDR $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/sensor/config/WDR/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.37/goke
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/cma_osal.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_acodec.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_adec.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_aenc.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_ai.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_aio.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_ao.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_base.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_chnl.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_h264e.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_h265e.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_isp.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_ive.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_jpege.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_rc.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_rgn.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_sys.ko
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_tde.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_vedu.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_venc.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_vgs.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_vi.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_vpss.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/gk7205v200_wdt.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/hi_mipi_rx.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/hi_osal.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/hi_pwm.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/hi_sensor_i2c.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/hi_sensor_spi.ko
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/goke $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/kmod/sys_config.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	#$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/script/load*
	#$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/script/ircut_demo
	#$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/script/set_allocator

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/fonts
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/fonts $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/fonts/*.bin

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libaaccomm.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libaacdec.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libaacenc.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libaacsbrdec.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libaacsbrenc.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libbcd.so
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libdnvqe.so
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/lib_hiae.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/lib_hiawb_natura.so
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/lib_hiawb.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhi_cipher.so
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/lib_hidehaze.so
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/lib_hidrc.so
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/lib_hiir_auto.so
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/lib_hildci.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhive_AEC.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhive_AGC.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhive_ANR.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhive_common.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhive_EQ.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhive_HPF.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhive_record.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libhive_RES.so
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libisp.so
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libive.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libivp.so
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libmd.so
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libmpi.so
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libsecurec.so
	## $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libtde.so
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libupvqe.so
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_GOKE_PATH)/package/goke-osdrv-gk7205v200/files/lib/libVoiceEngine.so
endef

$(eval $(generic-package))
