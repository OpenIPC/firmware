################################################################################
#
# rtl8192eu
#
################################################################################

RTL8192EU_OPENIPC_VERSION = 5484fbd00c39826fd91e2d784cce211d55537447
RTL8192EU_OPENIPC_SITE = $(call github,Mange,rtl8192eu-linux-driver,$(RTL8192EU_OPENIPC_VERSION))
RTL8192EU_OPENIPC_LICENSE = GPL-2.0
RTL8192EU_OPENIPC_LICENSE_FILES = COPYING
RTL8192EU_OPENIPC_MODULE_MAKE_OPTS = CONFIG_RTL8192EU=m

define RTL8192EU_OPENIPC_INSTALL_FIRMWARE
	$(INSTALL) -D -m 644 ../general/package/rtl8192eu-openipc/rtl8192her.bin \
		$(TARGET_DIR)/lib/firmware/rtlwifi/rtl8192her.bin
endef

# RTL8192EU_OPENIPC_POST_INSTALL_TARGET_HOOKS += RTL8192EU_OPENIPC_INSTALL_FIRMWARE

$(eval $(kernel-module))
$(eval $(generic-package))
