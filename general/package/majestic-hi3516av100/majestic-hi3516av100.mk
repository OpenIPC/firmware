################################################################################
#
# majestic-hi3516c2300
#
################################################################################

MAJESTIC_HI3516AV100_VERSION = current
MAJESTIC_HI3516AV100_SOURCE = majestic.hi3516av100.lite.master.tar.bz2
MAJESTIC_HI3516AV100_SITE = https://openipc.s3-eu-west-1.amazonaws.com
MAJESTIC_HI3516AV100_LICENSE = MIT
MAJESTIC_HI3516AV100_LICENSE_FILES = LICENSE

define MAJESTIC_HI3516AV100_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 $(@D)/majestic-mini.yaml $(TARGET_DIR)/etc/majestic.yaml
	$(INSTALL) -m 644 $(@D)/majestic.yaml $(TARGET_DIR)/etc/majestic.full

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/majestic
endef

$(eval $(generic-package))
