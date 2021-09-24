################################################################################
#
# majestic-fonts
#
################################################################################

MAJESTIC_FONTS_VERSION = current
MAJESTIC_FONTS_SOURCE = fonts.tar.bz2
MAJESTIC_FONTS_SITE = http://openipc.s3-eu-west-1.amazonaws.com
MAJESTIC_FONTS_LICENSE = MIT
MAJESTIC_FONTS_LICENSE_FILES = LICENSE

define MAJESTIC_FONTS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/fonts
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/fonts $(@D)/fonts.bin
endef


$(eval $(generic-package))
