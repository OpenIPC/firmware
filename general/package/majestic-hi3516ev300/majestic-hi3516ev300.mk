################################################################################
#
# majestic-hi3516ev300
#
################################################################################

MAJESTIC_HI3516EV300_VERSION = current
MAJESTIC_HI3516EV300_SOURCE = majestic.hi3516ev300.lite.master.tar.bz2
MAJESTIC_HI3516EV300_SITE = https://openipc.s3-eu-west-1.amazonaws.com
MAJESTIC_HI3516EV300_LICENSE = MIT
MAJESTIC_HI3516EV300_LICENSE_FILES = LICENSE

#ifeq ($(BR2_PACKAGE_MAJESTIC_HI3516EV300_ULTIMATE),y)
#MAJESTIC_HI3516EV300_SOURCE = majestic.hi3516ev300.ultimate.master.tar.bz2
#else
#MAJESTIC_HI3516EV300_SOURCE = majestic.hi3516ev300.master.tar.bz2
#endif

define MAJESTIC_HI3516EV300_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 $(@D)/majestic-mini.yaml $(TARGET_DIR)/etc/majestic.yaml
	$(INSTALL) -m 644 $(@D)/majestic.yaml $(TARGET_DIR)/etc/majestic.full

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/majestic
endef

$(eval $(generic-package))
