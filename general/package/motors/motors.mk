################################################################################
#
# motors | updated 2023.05.11
#
################################################################################

MOTORS_VERSION = 63e955dc0d3e048f801bd4ee2fcdc1dbfe562c0c
MOTORS_SITE = $(call github,openipc,motors,$(MOTORS_VERSION))
MOTORS_LICENSE = MIT
MOTORS_LICENSE_FILES = LICENSE

define MOTORS_BUILD_CMDS
	(cd $(@D)/camhi-motor; $(TARGET_CC) -Os -s main.c -o camhi-motor)
	(cd $(@D)/i2c-motor; $(TARGET_CC) -Os -s main.c -o i2c-motor)
	(cd $(@D)/xm-kmotor; $(TARGET_CC) -Os -s main.c -o xm-kmotor)
	(cd $(@D)/xm-uart; $(TARGET_CC) -Os -s main.c -o xm-uart)
endef

define MOTORS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/camhi-motor/camhi-motor $(TARGET_DIR)/usr/bin/camhi-motor
	$(INSTALL) -m 0755 -D $(@D)/i2c-motor/i2c-motor $(TARGET_DIR)/usr/bin/i2c-motor
	$(INSTALL) -m 0755 -D $(@D)/xm-kmotor/xm-kmotor $(TARGET_DIR)/usr/bin/xm-kmotor
	$(INSTALL) -m 0755 -D $(@D)/xm-uart/xm-uart $(TARGET_DIR)/usr/bin/xm-uart
endef

$(eval $(generic-package))
