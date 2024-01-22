################################################################################
#
# ingenic-pwm
#
################################################################################

INGENIC_PWM_SITE = $(call github,gtxaspec,ingenic-pwm,$(INGENIC_PWM_VERSION))
INGENIC_PWM_VERSION = master

define INGENIC_PWM_BUILD_CMDS
    $(MAKE) CROSS_COMPILE=$(TARGET_CROSS) -C $(@D)
endef

define INGENIC_PWM_INSTALL_TARGET_CMDS
    $(INSTALL) -D -m 0755 $(@D)/ingenic-pwm $(TARGET_DIR)/usr/bin/
endef

$(eval $(generic-package))
