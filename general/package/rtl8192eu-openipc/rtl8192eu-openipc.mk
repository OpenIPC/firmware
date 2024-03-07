################################################################################
#
# rtl8192eu-openipc
#
################################################################################

RTL8192EU_OPENIPC_SITE = $(call github,mange,rtl8192eu-linux-driver,$(RTL8192EU_OPENIPC_VERSION))
RTL8192EU_OPENIPC_VERSION = HEAD

RTL8192EU_OPENIPC_LICENSE = GPL-2.0
RTL8192EU_OPENIPC_LICENSE_FILES = COPYING
RTL8192EU_OPENIPC_MODULE_MAKE_OPTS = CONFIG_RTL8192EU=m

define RTL8192EU_OPENIPC_INSTALL_FIRMWARE
	$(INSTALL) -D -m 644 $(RTL8192EU_OPENIPC_PKGDIR)/rtl8192her.bin \
		$(TARGET_DIR)/lib/firmware/rtlwifi/rtl8192her.bin
endef

$(eval $(kernel-module))
$(eval $(generic-package))
