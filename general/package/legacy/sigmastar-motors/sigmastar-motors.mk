################################################################################
#
# sigmastar-motors
#
################################################################################

SIGMASTAR_MOTORS_SITE_METHOD = local
SIGMASTAR_MOTORS_SITE = $(SIGMASTAR_MOTORS_PKGDIR)/src

define SIGMASTAR_MOTORS_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) -C $(@D)
endef

define SIGMASTAR_MOTORS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/output/*
endef

$(eval $(generic-package))
