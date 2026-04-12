################################################################################
#
# autonight
#
################################################################################

AUTONIGHT_SITE_METHOD = local
AUTONIGHT_SITE = $(AUTONIGHT_PKGDIR)/src

AUTONIGHT_LICENSE = MIT
AUTONIGHT_LICENSE_FILES = LICENSE

define AUTONIGHT_BUILD_CMDS
	$(TARGET_CC) $(@D)/autonight.c -o $(@D)/autonight -s
endef

define AUTONIGHT_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -t $(TARGET_DIR)/usr/bin $(@D)/autonight
endef

$(eval $(generic-package))
