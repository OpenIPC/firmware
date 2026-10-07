################################################################################
#
# motors
#
################################################################################

MOTORS_SITE = $(call github,openipc,motors,$(MOTORS_VERSION))
MOTORS_VERSION = HEAD

MOTORS_LICENSE = MIT
MOTORS_LICENSE_FILES = LICENSE

# xm-uart, the standalone XM lens-board UART tool, is deliberately not built.
# majestic-af is the driver for that lens now and must be the only writer on
# its tty; a second tool opening the same port behind the daemon's back is the
# race majestic-af's single-owner rework removed, and nothing on the image
# calls xm-uart any more.

define MOTORS_BUILD_CMDS
	(cd $(@D)/camhi-motor; $(TARGET_CC) -Os -s main.c -o camhi-motor)
	(cd $(@D)/i2c-motor; $(TARGET_CC) -Os -s main.c -o i2c-motor)
	(cd $(@D)/xm-kmotor; $(TARGET_CC) -Os -s main.c -o xm-kmotor)
	(cd $(@D)/ingenic-motor; $(TARGET_CC) -Os -s main.c -o ingenic-motor)
endef

define MOTORS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/camhi-motor/camhi-motor $(TARGET_DIR)/usr/bin/camhi-motor
	$(INSTALL) -m 0755 -D $(@D)/i2c-motor/i2c-motor $(TARGET_DIR)/usr/bin/i2c-motor
	$(INSTALL) -m 0755 -D $(@D)/xm-kmotor/xm-kmotor $(TARGET_DIR)/usr/bin/xm-kmotor
	$(INSTALL) -m 0755 -D $(@D)/ingenic-motor/ingenic-motor $(TARGET_DIR)/usr/bin/ingenic-motor
endef

$(eval $(generic-package))
