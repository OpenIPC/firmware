################################################################################
#
# majestic-fonts
#
################################################################################

# files/OpenIPCMono-Regular.ttf is a subset of Noto Sans Mono
# 2.014: ASCII, Cyrillic, the degree sign and the numero sign. make-subset.py
# remakes it byte for byte from the upstream font; anything majestic draws
# outside it renders as an empty box. UbuntuMono-Regular.ttf, the old name, is
# a symlink to it: osd.font in deployed configs, msposd and divinus use it.
MAJESTIC_FONTS_VERSION = 0.14
MAJESTIC_FONTS_SITE_METHOD = local
MAJESTIC_FONTS_SITE = $(MAJESTIC_FONTS_PKGDIR)/files

MAJESTIC_FONTS_LICENSE = OFL-1.1
MAJESTIC_FONTS_LICENSE_FILES = LICENSE

define MAJESTIC_FONTS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/fonts/truetype
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts/truetype $(@D)/OpenIPCMono-Regular.ttf
	ln -sf OpenIPCMono-Regular.ttf $(TARGET_DIR)/usr/share/fonts/truetype/UbuntuMono-Regular.ttf
	$(INSTALL) -m 644 $(@D)/LICENSE $(TARGET_DIR)/usr/share/fonts/truetype/OpenIPCMono-Regular.LICENSE
endef

$(eval $(generic-package))
