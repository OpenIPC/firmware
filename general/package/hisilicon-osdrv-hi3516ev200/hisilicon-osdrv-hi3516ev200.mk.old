################################################################################
#
# hisilicon-osdrv-hi3516ev200
#
################################################################################

HISILICON_OSDRV_HI3516EV200_VERSION =
HISILICON_OSDRV_HI3516EV200_SITE =
HISILICON_OSDRV_HI3516EV200_LICENSE = MIT
HISILICON_OSDRV_HI3516EV200_LICENSE_FILES = LICENSE

define HISILICON_OSDRV_HI3516EV200_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/WDR
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/WDR $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/sensor/config/WDR/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/iq
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/sensor/iq/imx307.ini
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/sensor/iq/imx335.ini
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/sensor/iq/f23.ini
	ln -sf imx307.ini $(TARGET_DIR)/etc/sensors/iq/default.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/hisilicon
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/hisilicon $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/kmod/camhi-motor.ko
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/hisilicon $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/kmod/hi3516ev200_isp.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/script/ircut_demo
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/script/set_allocator

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libaaccomm.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libaacdec.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libaacenc.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libaacsbrdec.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libaacsbrenc.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libbcd.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libdnvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/lib_hiae.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/lib_hiawb_natura.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/lib_hiawb.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libhi_cipher.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/lib_hidehaze.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/lib_hidrc.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/lib_hiir_auto.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/lib_hildci.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libhive_AEC.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libhive_AGC.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libhive_ANR.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libhive_common.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libhive_EQ.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libhive_HPF.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libhive_record.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libhive_RES.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libisp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libive.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libivp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libmd.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libmpi.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libsecurec.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libtde.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libupvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libVoiceEngine.so
endef

$(eval $(generic-package))
