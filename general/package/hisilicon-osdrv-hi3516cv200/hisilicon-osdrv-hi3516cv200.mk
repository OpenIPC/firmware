################################################################################
#
# hisilicon-osdrv-hi3516cv200
#
################################################################################

HISILICON_OSDRV_HI3516CV200_VERSION =
HISILICON_OSDRV_HI3516CV200_SITE =
HISILICON_OSDRV_HI3516CV200_LICENSE = MIT
HISILICON_OSDRV_HI3516CV200_LICENSE_FILES = LICENSE

define HISILICON_OSDRV_HI3516CV200_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/iq
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/iq/*.ini
	ln -sf ar0130.ini $(TARGET_DIR)/etc/sensors/iq/default.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.37/hisilicon

	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/acodec.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/adv_7179.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_adec.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_aenc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_ai.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_aio.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_ao.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_base.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_chnl.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_h264e.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_isp.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_ive.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_jpege.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_rc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_region.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_sys.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_tde.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_venc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_vgs.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_viu.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_vou.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi3518e_vpss.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi_cipher.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hifb.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi_media.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi_mipi.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hi_rtc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/hiuser.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/mmz.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/piris.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/pwm.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/sample_ist.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/sensor_i2c.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/sensor_spi.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/sys_config.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/wdt.ko

	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/kmod/xxx.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/script/ircut_demo

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/*.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_9m034.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_ar0130.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_ar0230.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_ar0237.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_gc1034.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_gc2023.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_gc2033.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_imx122.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_imx222.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_imx291.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_imx307.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_imx323_i2c_dc.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_imx323_i2c_dc_v1.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_imx323_i2c_dc_v2.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_imx323_i2c_dc_v3.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_imx327.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_jxf22.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_jxf23.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_jxh62.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_jxh65.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_mn34222.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_ov2718.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_ov2735.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_ov9712.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_ov9732.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_ov9750.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_ov9752.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_ov9750_v2.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_sc1135.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_sc1145.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_sc1235.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_sc2035.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_sc2135.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_sc2232.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/sensor/libsns_sc2235.so
	ln -sf libsns_imx323_i2c_dc_v1.so $(TARGET_DIR)/usr/lib/sensors/libsns_imx323_i2c_dc.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib

	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libdnvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/lib_hiae.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/lib_hiaf.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/lib_hiawb.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/lib_hidefog.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/lib_hiirauto.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libhive_AEC.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libhive_AGC.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libhive_ANR.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libhive_common.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libhive_EQ.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libhive_GAIN.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libhive_HDR.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libhive_HPF.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libhive_MBC.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libhive_RES.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libhive_RNR.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libisp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libive.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libmd.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libmpi.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libslice_trans.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libtde.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libupvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/libVoiceEngine.so

	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV200_PKGDIR)/files/lib/xxx.so
endef

$(eval $(generic-package))
