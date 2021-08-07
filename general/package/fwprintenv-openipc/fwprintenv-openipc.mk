################################################################################
#
# fwprintenv-openipc
#
################################################################################

FWPRINTENV_OPENIPC_VERSION = 2020.04
FWPRINTENV_OPENIPC_SOURCE = u-boot-$(FWPRINTENV_OPENIPC_VERSION).tar.bz2
FWPRINTENV_OPENIPC_SITE = ftp://ftp.denx.de/pub/u-boot
FWPRINTENV_OPENIPC_LICENSE = GPL-2.0+
FWPRINTENV_OPENIPC_LICENSE_FILES = Licenses/gpl-2.0.txt
FWPRINTENV_OPENIPC_CPE_ID_VENDOR = denx
FWPRINTENV_OPENIPC_CPE_ID_PRODUCT = u-boot
FWPRINTENV_OPENIPC_INSTALL_STAGING = YES

define FWPRINTENV_OPENIPC_CONFIGURE_CMDS
	mkdir -p $(@D)/include/config
	touch $(@D)/include/config/auto.conf
endef

FWPRINTENV_OPENIPC_MAKE_OPTS = CROSS_COMPILE="$(TARGET_CROSS)" \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	HOSTCFLAGS="$(HOST_CFLAGS)" \
	STRIP=$(TARGET_STRIP)

define FWPRINTENV_OPENIPC_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(BR2_MAKE) -C $(@D) $(FWPRINTENV_OPENIPC_MAKE_OPTS) \
		CROSS_BUILD_TOOLS=y tools-only
	$(TARGET_MAKE_ENV) $(BR2_MAKE) -C $(@D) $(UBOOT_TOOLS_MAKE_OPTS) \
		envtools no-dot-config-targets=envtools
endef

define FWPRINTENV_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/tools/env/fw_printenv $(TARGET_DIR)/usr/sbin/fw_printenv
	ln -sf fw_printenv $(TARGET_DIR)/usr/sbin/fw_setenv
endef

$(eval $(generic-package))
