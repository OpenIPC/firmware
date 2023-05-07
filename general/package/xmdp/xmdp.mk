################################################################################
#
# xmdp
#
################################################################################

XMDP_LICENSE = Public Domain

define XMDP_EXTRACT_CMDS
	cp -avr $(XMDP_PKGDIR)/src/* $(@D)/
endef

XMDP_MAKE_OPTS = \
	CC="$(TARGET_CC)"

define XMDP_BUILD_CMDS
	$(MAKE) $(XMDP_MAKE_OPTS) -C $(@D)
endef

define XMDP_INSTALL_TARGET_CMDS
	install -m 0755 -D $(@D)/xmdp $(TARGET_DIR)/usr/bin/xmdp
endef

$(eval $(generic-package))
