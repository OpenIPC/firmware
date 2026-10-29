################################################################################
#
# majestic-fonts
#
################################################################################

# files/UbuntuMono-Regular.ttf ("OpenIPC Mono") is a subset of Ubuntu Mono
# 0.80: ASCII, Cyrillic, the degree sign and the numero sign. make-subset.py
# remakes it byte for byte from the upstream font; anything majestic draws
# outside it renders as an empty box.
MAJESTIC_FONTS_VERSION = 0.13
MAJESTIC_FONTS_SITE_METHOD = local
MAJESTIC_FONTS_SITE = $(MAJESTIC_FONTS_PKGDIR)/files

MAJESTIC_FONTS_LICENSE = UFL-1.0
MAJESTIC_FONTS_LICENSE_FILES = LICENSE

define MAJESTIC_FONTS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/fonts/truetype
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts/truetype $(@D)/UbuntuMono-Regular.ttf
	$(INSTALL) -m 644 $(@D)/LICENSE $(TARGET_DIR)/usr/share/fonts/truetype/UbuntuMono-Regular.LICENSE
endef

$(eval $(generic-package))
