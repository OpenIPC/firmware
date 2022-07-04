################################################################################
#
# majestic-gk7205v200
#
################################################################################

MAJESTIC_GK7205V200_VERSION = current
MAJESTIC_GK7205V200_SOURCE = majestic.gk7205v200.lite.master.tar.bz2
MAJESTIC_GK7205V200_SITE = https://openipc.s3-eu-west-1.amazonaws.com
MAJESTIC_GK7205V200_LICENSE = MIT
MAJESTIC_GK7205V200_LICENSE_FILES = LICENSE

ifeq ($(BR2_PACKAGE_MAJESTIC_ULTIMATE),y)
MAJESTIC_GK7205V200_SOURCE = majestic.gk7205v200.ultimate.master.tar.bz2
else ifeq ($(BR2_PACKAGE_MAJESTIC_FPV),y)
MAJESTIC_GK7205V200_SOURCE = majestic.gk7205v200.fpv.master.tar.bz2
else
MAJESTIC_GK7205V200_SOURCE = majestic.gk7205v200.lite.master.tar.bz2
endif

define MAJESTIC_GK7205V200_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 $(@D)/majestic-mini.yaml $(TARGET_DIR)/etc/majestic.yaml
	$(INSTALL) -m 644 $(@D)/majestic.yaml $(TARGET_DIR)/etc/majestic.full

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/majestic
endef

$(eval $(generic-package))
