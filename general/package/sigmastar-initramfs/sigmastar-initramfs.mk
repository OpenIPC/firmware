################################################################################
#
# sigmastar-initramfs
#
################################################################################

SIGMASTAR_INITRAMFS_VERSION = 1.0
SIGMASTAR_INITRAMFS_SITE =
SIGMASTAR_INITRAMFS_SOURCE =

SIGMASTAR_INITRAMFS_PATH = $(SIGMASTAR_INITRAMFS_PKGDIR)/files/script

define SIGMASTAR_INITRAMFS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(SIGMASTAR_INITRAMFS_PATH)/S99inotifyd

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(SIGMASTAR_INITRAMFS_PATH)/savesettings
endef

$(eval $(generic-package))
