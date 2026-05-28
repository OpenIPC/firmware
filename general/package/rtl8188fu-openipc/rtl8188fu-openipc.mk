################################################################################
#
# rtl8188fu-openipc
#
################################################################################

RTL8188FU_OPENIPC_SITE = $(call github,openipc,realtek-wlan,$(RTL8188FU_OPENIPC_VERSION))
RTL8188FU_OPENIPC_VERSION = rtl8188fu

RTL8188FU_OPENIPC_LICENSE = GPL-2.0, proprietary (rtl8188fufw.bin firmware blob)
RTL8188FU_OPENIPC_LICENSE_FILES = COPYING

RTL8188FU_OPENIPC_MODULE_MAKE_OPTS = \
	CONFIG_RTL8188FU=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

define RTL8188FU_OPENIPC_INSTALL_FIRMWARE
	$(INSTALL) -D -m 644 $(RTL8188FU_OPENIPC_PKGDIR)/rtl8188fufw.bin \
		$(TARGET_DIR)/lib/firmware/rtlwifi/rtl8188fufw.bin
endef
RTL8188FU_OPENIPC_POST_INSTALL_TARGET_HOOKS += RTL8188FU_OPENIPC_INSTALL_FIRMWARE

$(eval $(kernel-module))
$(eval $(generic-package))
