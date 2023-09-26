################################################################################
#
# autonight
#
################################################################################

AUTONIGHT_LICENSE = MIT
AUTONIGHT_LICENSE_FILES = LICENSE

define AUTONIGHT_EXTRACT_CMDS
	cp -avr $(AUTONIGHT_PKGDIR)/src/* $(@D)/
endef

AUTONIGHT_MAKE_OPTS = \
	CC="$(TARGET_CC)"

define AUTONIGHT_BUILD_CMDS
	$(MAKE) $(AUTONIGHT_MAKE_OPTS) -C $(@D)
endef

define AUTONIGHT_INSTALL_TARGET_CMDS
	install -m 0755 -D $(@D)/autonight $(TARGET_DIR)/usr/bin/autonight
endef

$(eval $(generic-package))
