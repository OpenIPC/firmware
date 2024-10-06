################################################################################
#
# gk7205v200-motors
#
################################################################################

GK7205V200_MOTORS_SITE_METHOD = local
GK7205V200_MOTORS_SITE = $(GK7205V200_MOTORS_PKGDIR)/src

define GK7205V200_MOTORS_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) -C $(@D)
endef

define GK7205V200_MOTORS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/output/motor
endef

$(eval $(generic-package))
