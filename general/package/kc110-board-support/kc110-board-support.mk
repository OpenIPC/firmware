################################################################################
#
# kc110-board-support
#
################################################################################

KC110_BOARD_SUPPORT_SITE_METHOD = local
KC110_BOARD_SUPPORT_SITE = $(KC110_BOARD_SUPPORT_PKGDIR)/src

define KC110_BOARD_SUPPORT_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" CFLAGS="$(TARGET_CFLAGS)" -C $(@D)
endef

define KC110_BOARD_SUPPORT_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/ptz $(TARGET_DIR)/usr/bin/ptz
	cp -a $(KC110_BOARD_SUPPORT_PKGDIR)/files/. $(TARGET_DIR)/
endef

$(eval $(generic-package))
