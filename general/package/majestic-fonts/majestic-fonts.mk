################################################################################
#
# majestic-fonts | updated 2021.12.04
#
################################################################################

#MAJESTIC_FONTS_VERSION = current
MAJESTIC_FONTS_LICENSE = MIT
MAJESTIC_FONTS_LICENSE_FILES = LICENSE

define MAJESTIC_FONTS_EXTRACT_CMDS
	cp ../general/package/majestic-fonts/files/* $(@D)/
endef

define MAJESTIC_FONTS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/fonts/truetype
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts/truetype $(@D)/UbuntuMono-Regular.ttf
endef

$(eval $(generic-package))
