################################################################################
#
# hisilicon-osdrv-hi3516cv100
#
################################################################################

HISILICON_OSDRV_HI3516CV100_VERSION =
HISILICON_OSDRV_HI3516CV100_SITE =
HISILICON_OSDRV_HI3516CV100_LICENSE = MIT
HISILICON_OSDRV_HI3516CV100_LICENSE_FILES = LICENSE

define HISILICON_OSDRV_HI3516CV100_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/script/S95hisilicon

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/iq
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/sensor/iq/default.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.0.8/hisilicon
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/acodec.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/gpioi2c_ex.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/gpioi2c.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_adec.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_aenc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_ai.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_ao.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_base.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_chnl.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_dsu.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_group.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_h264e.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_isp.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_ive.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_jpege.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_rc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_region.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_sio.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_sys.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_tde.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_vda.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_venc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_viu.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_vou.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi3518_vpss.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hidmac.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hifb.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi_i2c.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hi_rtc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/hiuser.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/mmz.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/pwm.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/sil9024.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/ssp_ad9020.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/ssp_pana.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/ssp_sony.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/kmod/wdt.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/script/load_hisilicon
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/script/ircut_demo

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/libaec.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/libanr.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/libdnvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/lib_hiae.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/lib_hiae_v2.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/lib_hiaf.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/lib_hiawb.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/libhive_AEC.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/libhive_AGC.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/libhive_ANR.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/libhive_common.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/libhive_EQ.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/libhive_GAIN.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/libhive_HPF.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/libhive_RES.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/libisp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/libmem.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/libmpi.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/libresampler.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/libtde.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/libupvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/libVoiceEngine.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(BR2_EXTERNAL_HISILICON_PATH)/package/hisilicon-osdrv-hi3516cv100/files/lib/libvqev2.so
endef

$(eval $(generic-package))
