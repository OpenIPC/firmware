################################################################################
#
# hisilicon-osdrv-hi3516av100
#
################################################################################

HISILICON_OSDRV_HI3516AV100_VERSION =
HISILICON_OSDRV_HI3516AV100_SITE =
HISILICON_OSDRV_HI3516AV100_LICENSE = MIT
HISILICON_OSDRV_HI3516AV100_LICENSE_FILES = LICENSE

define HISILICON_OSDRV_HI3516AV100_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/sensor/config/*.ini

	#$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/WDR
	#$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/WDR $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/sensor/config/WDR/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.37/hisilicon
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/acodec.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_adec.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_aenc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_ai.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_aio.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_ao.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_base.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_chnl.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_h264e.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_h265e.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_isp.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_ive.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_jpege.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_pm.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_rc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_region.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_sys.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_tde.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_vda.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_venc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_vgs.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_viu.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_vou.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi3516a_vpss.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hifb.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi_media.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi_mipi.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hi_rtc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/hiuser.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/mmz.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/piris.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/pwm.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/sensor_i2c.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/sensor_spi.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/kmod/wdt.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/script/ircut_demo
	#$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/script/set_allocator

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libdnvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/lib_hiae.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/lib_hiaf.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/lib_hiawb.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/lib_hidefog.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libhive_AEC.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libhive_AGC.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libhive_ANR.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libhive_common.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libhive_DRC.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libhive_EQ.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libhive_GAIN.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libhive_HDR.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libhive_HPF.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libhive_MBC.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libhive_PEQ.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libhive_RES.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libhive_RNR.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libisp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libive.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libmpi.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libslice_trans.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libtde.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libupvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516AV100_PKGDIR)/files/lib/libVoiceEngine.so
endef

$(eval $(generic-package))
