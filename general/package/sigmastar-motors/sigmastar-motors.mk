################################################################################
#
# sigmastar-motors
#
################################################################################

define SIGMASTAR_MOTORS_EXTRACT_CMDS
	cp -r $(SIGMASTAR_MOTORS_PKGDIR)/src/* $(@D)
endef

define SIGMASTAR_MOTORS_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) -C $(@D)
endef

define SIGMASTAR_MOTORS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/output/*
endef

$(eval $(generic-package))
