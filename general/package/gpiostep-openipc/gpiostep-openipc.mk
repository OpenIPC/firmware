################################################################################
#
# gpiostep-openipc
#
################################################################################

GPIOSTEP_OPENIPC_VERSION = 1.0
GPIOSTEP_OPENIPC_SITE_METHOD = local
GPIOSTEP_OPENIPC_SITE = $(GPIOSTEP_OPENIPC_PKGDIR)/src
GPIOSTEP_OPENIPC_LICENSE = GPL-2.0

# Build the userspace CLI; the .ko is built/installed by the kernel-module infra.
define GPIOSTEP_OPENIPC_BUILD_CMDS
	$(TARGET_CC) -Os -s $(@D)/gpiostep-ctl.c -o $(@D)/gpiostep-ctl
endef

define GPIOSTEP_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/gpiostep-ctl $(TARGET_DIR)/usr/bin/gpiostep-ctl
endef

# The module's built-in pin map is the GK7205V510's, and those GPIO numbers
# mean other pads on other SoCs, so only a Goke build gets it. Elsewhere the
# board's pins are given at insmod, and a load without them is refused.
ifeq ($(call qstrip,$(BR2_OPENIPC_SOC_VENDOR)),goke)
GPIOSTEP_OPENIPC_MODULE_MAKE_OPTS += GPIOSTEP_CFLAGS=-DGPIOSTEP_GK7205V510_PINS
endif

$(eval $(kernel-module))
$(eval $(generic-package))
