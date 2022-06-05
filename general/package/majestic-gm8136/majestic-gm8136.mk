################################################################################
#
# majestic-gm8136
#
################################################################################

MAJESTIC_GM8136_VERSION = current
MAJESTIC_GM8136_SOURCE = majestic.gm8136.lite.master.tar.bz2
MAJESTIC_GM8136_SITE = https://openipc.s3-eu-west-1.amazonaws.com
MAJESTIC_GM8136_LICENSE = MIT
MAJESTIC_GM8136_LICENSE_FILES = LICENSE

define MAJESTIC_GM8136_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 $(@D)/majestic-mini.yaml $(TARGET_DIR)/etc/majestic.yaml
	$(INSTALL) -m 644 $(@D)/majestic.yaml $(TARGET_DIR)/etc/majestic.full

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/majestic
endef

$(eval $(generic-package))
