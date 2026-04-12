################################################################################
#
# majestic-fonts
#
################################################################################

MAJESTIC_FONTS_LICENSE = MIT
MAJESTIC_FONTS_LICENSE_FILES = LICENSE

define MAJESTIC_FONTS_EXTRACT_CMDS
	cp $(MAJESTIC_FONTS_PKGDIR)/files/* $(@D)/
endef

define MAJESTIC_FONTS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/fonts/truetype
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts/truetype $(@D)/UbuntuMono-Regular.ttf
endef

$(eval $(generic-package))
