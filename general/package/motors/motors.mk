################################################################################
#
# motors
#
################################################################################

ifeq ($(LOCAL_DOWNLOAD),y)
MOTORS_SITE_METHOD = git
MOTORS_SITE = https://github.com/openipc/motors
MOTORS_VERSION = $(shell git ls-remote $(MOTORS_SITE) HEAD | head -1 | cut -f1)
else
MOTORS_SITE = https://github.com/openipc/motors/archive
MOTORS_SOURCE = master.tar.gz
endif

MOTORS_LICENSE = MIT
MOTORS_LICENSE_FILES = LICENSE

define MOTORS_BUILD_CMDS
	(cd $(@D)/camhi-motor; $(TARGET_CC) -Os -s main.c -o camhi-motor)
	(cd $(@D)/i2c-motor; $(TARGET_CC) -Os -s main.c -o i2c-motor)
	(cd $(@D)/xm-kmotor; $(TARGET_CC) -Os -s main.c -o xm-kmotor)
	(cd $(@D)/xm-uart; $(TARGET_CC) -Os -s main.c -o xm-uart)
	(cd $(@D)/t31-kmotor; $(TARGET_CC) -Os -s main.c -o t31-kmotor)
endef

define MOTORS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/camhi-motor/camhi-motor $(TARGET_DIR)/usr/bin/camhi-motor
	$(INSTALL) -m 0755 -D $(@D)/i2c-motor/i2c-motor $(TARGET_DIR)/usr/bin/i2c-motor
	$(INSTALL) -m 0755 -D $(@D)/xm-kmotor/xm-kmotor $(TARGET_DIR)/usr/bin/xm-kmotor
	$(INSTALL) -m 0755 -D $(@D)/xm-uart/xm-uart $(TARGET_DIR)/usr/bin/xm-uart
	$(INSTALL) -m 0755 -D $(@D)/t31-kmotor/t31-kmotor $(TARGET_DIR)/usr/bin/t31-kmotor
endef

$(eval $(generic-package))
