################################################################################
#
# rtl8192eu
#
################################################################################

RTL8192EU_OPENIPC_SITE_METHOD = git
RTL8192EU_OPENIPC_SITE = https://github.com/mange/rtl8192eu-linux-driver
RTL8192EU_OPENIPC_VERSION = $(shell git ls-remote $(RTL8192EU_OPENIPC_SITE) HEAD | head -1 | awk '{ print $$1 }')

RTL8192EU_OPENIPC_LICENSE = GPL-2.0
RTL8192EU_OPENIPC_LICENSE_FILES = COPYING
RTL8192EU_OPENIPC_MODULE_MAKE_OPTS = CONFIG_RTL8192EU=m

define RTL8192EU_OPENIPC_INSTALL_FIRMWARE
	$(INSTALL) -D -m 644 $(RTL8192EU_OPENIPC_PKGDIR)/rtl8192her.bin \
		$(TARGET_DIR)/lib/firmware/rtlwifi/rtl8192her.bin
endef

# RTL8192EU_OPENIPC_POST_INSTALL_TARGET_HOOKS += RTL8192EU_OPENIPC_INSTALL_FIRMWARE

$(eval $(kernel-module))
$(eval $(generic-package))
