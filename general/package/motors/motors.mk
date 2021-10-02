################################################################################
#
# motors | updated 2021.10.02
#
################################################################################

MOTORS_VERSION = 686c50a31370dcbb1a71fc8b499f247868a52436
MOTORS_SITE = $(call github,openipc,motors,$(MOTORS_VERSION))
HISI_GPIO_LICENSE = MIT
HISI_GPIO_LICENSE_FILES = LICENSE

define MOTORS_BUILD_CMDS
	(cd $(@D)/xm-kmotor; $(TARGET_CC) -Os -s main.c -o xm-kmotor)
	(cd $(@D)/xm-uart; $(TARGET_CC) -Os -s main.c -o xm-uart)
endef

define MOTORS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/xm-kmotor/xm-kmotor $(TARGET_DIR)/usr/bin/xm-kmotor
	$(INSTALL) -m 0755 -D $(@D)/xm-uart/xm-uart $(TARGET_DIR)/usr/bin/xm-uart
endef

$(eval $(generic-package))
