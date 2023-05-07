################################################################################
#
# hisilicon-osdrv-hi3516cv500
#
################################################################################

HISILICON_OSDRV_HI3516CV500_VERSION =
HISILICON_OSDRV_HI3516CV500_SITE =
HISILICON_OSDRV_HI3516CV500_LICENSE = MIT
HISILICON_OSDRV_HI3516CV500_LICENSE_FILES = LICENSE

define HISILICON_OSDRV_HI3516CV500_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/60fps
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/60fps $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/sensor/config/60fps/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.37/hisilicon
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_acodec.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_adec.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_aenc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_ai.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_aio.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_ao.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_base.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_chnl.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_dis.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_gdc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_h264e.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_h265e.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_hdmi.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_ir.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_isp.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_ive.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_jpegd.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_jpege.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_nnie.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_rc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_rgn.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_svprt.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_sys.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_tde.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_vdec.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_vedu.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_venc.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_vfmw.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_vgs.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_vi.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_vo.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_vpss.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi3516cv500_wdt.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi_cipher.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hifb.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi_mipi_rx.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi_mipi_tx.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi_osal.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi_piris.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi_pwm.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi_sample_ist.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi_sensor_i2c.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi_sensor_spi.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi_ssp_ota5182.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi_ssp_sony.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi_ssp_st7789.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi_tlv320aic31.ko
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/hi_user.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/kmod/sys_config.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/script/load*
	# $(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/script/ircut_demo
	# $(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/script/set_allocator

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libaaccomm.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libaacdec.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libaacenc.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libaacsbrdec.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libaacsbrenc.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libdnvqe.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libhdmi.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/lib_hiacs.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/lib_hiae.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/lib_hiawb_natura.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/lib_hiawb.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/lib_hicalcflicker.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libhi_cipher.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/lib_hidehaze.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/lib_hidrc.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libhifisheyecalibrate.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/lib_hiir_auto.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/lib_hildci.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libhive_AEC.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libhive_AGC.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libhive_ANR.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libhive_common.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libhive_EQ.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libhive_HPF.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libhive_record.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libhive_RES_ext.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libhive_RES.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libisp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libive.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libmd.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libmpi.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libnnie.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libsecurec.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libsvpruntime.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libtde.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libupvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libVoiceEngine.so

endef

$(eval $(generic-package))
