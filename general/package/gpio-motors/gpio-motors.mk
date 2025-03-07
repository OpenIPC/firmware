################################################################################
#
# gpio-motors
#
################################################################################

GPIO_MOTORS_SITE_METHOD = local
GPIO_MOTORS_SITE = $(GPIO_MOTORS_PKGDIR)/src

define GPIO_MOTORS_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) -C $(@D)
endef

define GPIO_MOTORS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/output/*
endef

$(eval $(generic-package))
