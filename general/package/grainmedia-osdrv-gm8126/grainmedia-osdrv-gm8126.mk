################################################################################
#
# grainmedia-osdrv-gm8126
#
################################################################################

# No vendor binaries: the drivers are the kernel's own modules.
GRAINMEDIA_OSDRV_GM8126_VERSION =
GRAINMEDIA_OSDRV_GM8126_SITE =
GRAINMEDIA_OSDRV_GM8126_LICENSE = MIT
GRAINMEDIA_OSDRV_GM8126_LICENSE_FILES = LICENSE

define GRAINMEDIA_OSDRV_GM8126_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(GRAINMEDIA_OSDRV_GM8126_PKGDIR)/files/script/load*
endef

$(eval $(generic-package))
