################################################################################
#
# hisilicon-osdrv-hi3516cv500
#
################################################################################

HISILICON_OSDRV_HI3516CV500_VERSION =
HISILICON_OSDRV_HI3516CV500_SITE =
HISILICON_OSDRV_HI3516CV500_LICENSE = MIT
HISILICON_OSDRV_HI3516CV500_LICENSE_FILES = LICENSE

# osdrv ships sensor configs, load scripts, and vendor-only userspace
# .so libs. Kernel modules come from openhisilicon-opensdk (open_*.ko).
# The 3.18-era prebuilt .ko fallback was removed — every modern OpenIPC
# build pairs osdrv with opensdk; the no-opensdk path was dead code.

define HISILICON_OSDRV_HI3516CV500_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/60fps
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/60fps $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/sensor/config/60fps/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/720p120fps
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/720p120fps $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/sensor/config/720p120fps/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/vga200fps
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/vga200fps $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/sensor/config/vga200fps/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/script/load*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libaaccomm.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libaacdec.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libaacenc.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libaacsbrdec.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libaacsbrenc.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libdnvqe.so
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
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libive.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libmd.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libmpi.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libnnie.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libsecurec.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libsvpruntime.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libtde.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libupvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libVoiceEngine.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
endef

$(eval $(generic-package))
