################################################################################
#
# hisilicon-osdrv-hi3516cv300
#
################################################################################

HISILICON_OSDRV_HI3516CV300_VERSION =
HISILICON_OSDRV_HI3516CV300_SITE =
HISILICON_OSDRV_HI3516CV300_LICENSE = MIT
HISILICON_OSDRV_HI3516CV300_LICENSE_FILES = LICENSE

define HISILICON_OSDRV_HI3516CV300_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/iq
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/sensor/iq/*.ini
	ln -sf imx323.ini $(TARGET_DIR)/etc/sensors/iq/default.ini

	# osdrv installs only the prebuilt vendor .ko's that opensdk does NOT
	# build for cv300. Every module listed below is loaded by the active
	# (non-commented) paths in files/script/load_hisilicon and has no
	# equivalent in openhisilicon's cv300 build:
	#   - hi3516cv300_vou.ko — VO subsystem; opensdk sets DISABLE_VO=1 for cv300
	#   - hi_piris.ko        — P-iris driver; not in openhisilicon cv300 kbuild
	# The rest (hi_adv7179, hi_cipher, hi_sample_ist, hi_ssp_*,
	# hi_tlv320aic31, virtualhifb) are installed for board-specific paths
	# that load_hisilicon leaves commented out by default — they survive
	# here as opt-in extras until each grows an open replacement or is
	# pruned from load_hisilicon.
	# All other open-replaced blobs (base/sys/isp/vpss/venc/... + hi_osal/
	# sys_config/hi_mipi/hi_acodec/hi_sensor_spi) used to be installed here
	# too, then immediately overwritten by hisilicon-opensdk at install
	# time. Modern OpenIPC builds pair osdrv with opensdk on every cv300
	# board, so the double-install was pure dead weight; removed.
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.18.20/hisilicon
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/kmod/hi3516cv300_vou.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/kmod/hi_adv7179.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/kmod/hi_cipher.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/kmod/hi_piris.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/kmod/hi_sample_ist.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/kmod/hi_ssp_ili9341v_6bit.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/kmod/hi_ssp_ota5182.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/kmod/hi_tlv320aic31.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.20/hisilicon $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/kmod/virtualhifb.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/script/ircut_demo

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/sensor/*.so
	ln -sf libsns_imx291_i2c_lvds.so $(TARGET_DIR)/usr/lib/sensors/libsns_imx290_i2c_lvds.so
	ln -sf libsns_imx291_i2c_lvds.so $(TARGET_DIR)/usr/lib/sensors/libsns_imx307_i2c_lvds.so
	ln -sf libsns_imx291_i2c_lvds.so $(TARGET_DIR)/usr/lib/sensors/libsns_imx327_i2c_lvds.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/libdnvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/lib_hiae.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/lib_hiaf.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/lib_hiawb.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/libhi_cipher.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/lib_hidefog.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/lib_hiirauto.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/libhive_AEC.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/libhive_AGC.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/libhive_ANR.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/libhive_common.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/libhive_EQ.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/libhive_GAIN.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/libhive_HDR.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/libhive_HPF.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/libhive_MBC.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/libhive_RES.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/libhive_RNR.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/libisp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/libive.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/libmd.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/libmpi.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/libupvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/lib/libVoiceEngine.so
endef

$(eval $(generic-package))
