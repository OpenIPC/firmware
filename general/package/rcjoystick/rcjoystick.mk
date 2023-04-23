################################################################################
#
# rcjoystick
#
################################################################################

RCJOYSTICK_LICENSE = GPL-2.0

define RCJOYSTICK_EXTRACT_CMDS
	cp -avr ../general/package/rcjoystick/src/* $(@D)/
endef

RCJOYSTICK_MAKE_OPTS = \
	CC="$(TARGET_CC)"

define RCJOYSTICK_BUILD_CMDS
	$(MAKE) $(RCJOYSTICK_MAKE_OPTS) -C $(@D)
endef

define RCJOYSTICK_INSTALL_TARGET_CMDS
	install -m 0755 -D $(@D)/rcjoystick $(TARGET_DIR)/usr/bin/rcjoystick
endef

$(eval $(generic-package))
