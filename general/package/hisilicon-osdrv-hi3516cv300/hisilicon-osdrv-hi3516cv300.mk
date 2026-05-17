################################################################################
#
# hisilicon-osdrv-hi3516cv300
#
################################################################################

HISILICON_OSDRV_HI3516CV300_VERSION =
HISILICON_OSDRV_HI3516CV300_SITE =
HISILICON_OSDRV_HI3516CV300_LICENSE = MIT
HISILICON_OSDRV_HI3516CV300_LICENSE_FILES = LICENSE

# osdrv ships sensor configs, load scripts, and vendor-only userspace
# .so libs. Kernel modules come from openhisilicon-opensdk (open_*.ko).
# The 3.18-era prebuilt .ko fallback was removed — every modern OpenIPC
# build pairs osdrv with opensdk; the no-opensdk path was dead code.

define HISILICON_OSDRV_HI3516CV300_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/iq
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(HISILICON_OSDRV_HI3516CV300_PKGDIR)/files/sensor/iq/*.ini
	ln -sf imx323.ini $(TARGET_DIR)/etc/sensors/iq/default.ini

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
