################################################################################
#
# gpio-motors
#
################################################################################

GPIOD_MOTORS_SITE_METHOD = local
GPIOD_MOTORS_SITE = $(GPIOD_MOTORS_PKGDIR)/src
GPIOD_MOTORS_DEPENDENCIES = libgpiod

define GPIOD_MOTORS_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)
endef

define GPIOD_MOTORS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/output/*
endef

$(eval $(generic-package))
