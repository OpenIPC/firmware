################################################################################
#
# majestic-fh8852v200
#
################################################################################

MAJESTIC_FH8852V200_VERSION = current
MAJESTIC_FH8852V200_SOURCE = majestic.fh8852v200.lite.master.tar.bz2
MAJESTIC_FH8852V200_SITE = https://openipc.s3-eu-west-1.amazonaws.com
MAJESTIC_FH8852V200_LICENSE = MIT
MAJESTIC_FH8852V200_LICENSE_FILES = LICENSE

define MAJESTIC_FH8852V200_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 $(@D)/majestic-mini.yaml $(TARGET_DIR)/etc/majestic.yaml
	$(INSTALL) -m 644 $(@D)/majestic.yaml $(TARGET_DIR)/etc/majestic.full

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/majestic
endef

$(eval $(generic-package))
