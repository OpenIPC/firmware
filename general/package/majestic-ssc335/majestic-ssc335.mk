################################################################################
#
# majestic-ssc335
#
################################################################################

MAJESTIC_SSC335_VERSION = current
MAJESTIC_SSC335_SOURCE = majestic.ssc335.lite.master.tar.bz2
MAJESTIC_SSC335_SITE = https://openipc.s3-eu-west-1.amazonaws.com
MAJESTIC_SSC335_LICENSE = MIT
MAJESTIC_SSC335_LICENSE_FILES = LICENSE

define MAJESTIC_SSC335_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 $(@D)/majestic-mini.yaml $(TARGET_DIR)/etc/majestic.yaml
	$(INSTALL) -m 644 $(@D)/majestic.yaml $(TARGET_DIR)/etc/majestic.full

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/majestic
endef


$(eval $(generic-package))
