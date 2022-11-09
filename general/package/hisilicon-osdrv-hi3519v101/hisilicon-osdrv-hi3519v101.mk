################################################################################
#
# hisilicon-osdrv-hi3519v101
#
################################################################################

HISILICON_OSDRV_HI3519V101_VERSION =
HISILICON_OSDRV_HI3519V101_SITE =
HISILICON_OSDRV_HI3519V101_LICENSE = MIT
HISILICON_OSDRV_HI3519V101_LICENSE_FILES = LICENSE

define HISILICON_OSDRV_HI3519V101_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/script/S95hisilicon

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.18.20/hisilicon
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_adec.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_aenc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_ai.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_aio.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_ao.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_base.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_chnl.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_fisheye.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_h264e.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_h265e.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_ir.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_isp.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_ive.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_jpege.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_pciv_fmw.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_pciv.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_photo.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_pm.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_rc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_region.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_rtc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_sys.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_tde.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_vedu.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_venc.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_vgs.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_viu.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_vou.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_vpss.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi3519v101_wdt.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi_acodec.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi_cipher.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hifb.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi_mipi.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi_osal.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi_piris.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi_pwm.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi_sensor_i2c.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi_sensor_spi.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi_ssp_sony.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/kmod/hi_user.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/script/load_hisilicon
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/script/ircut_demo

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/libdnvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/lib_hiae.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/lib_hiaf.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/lib_hiawb.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/libhi_cipher.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/lib_hidefog.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/lib_hiirauto.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/libhive_AEC.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/libhive_AGC.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/libhive_ANR.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/libhive_common.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/libhive_EQ.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/libhive_GAIN.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/libhive_HDR.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/libhive_HPF.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/libhive_MBC.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/libhive_RES.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/libhive_RNR.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/libisp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/libive.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/libmd.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/libmpi.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/libupvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3519v101/files/lib/libVoiceEngine.so
endef

$(eval $(generic-package))
