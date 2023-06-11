################################################################################
#
# rtl8188fu-openipc
#
################################################################################

RTL8188FU_OPENIPC_SITE_METHOD = git
RTL8188FU_OPENIPC_SITE = https://github.com/ulli-kroll/rtl8188fu
RTL8188FU_OPENIPC_VERSION = e2d69b2b0c911c1b59daaf59a582ccdbb73fd97d
#RTL8188FU_OPENIPC_VERSION = $(shell git ls-remote $(RTL8188FU_OPENIPC_SITE) HEAD | head -1 | awk '{ print $$1 }')

RTL8188FU_OPENIPC_LICENSE = GPL-2.0

RTL8188FU_OPENIPC_MODULE_MAKE_OPTS = \
	CONFIG_RTL8188FU=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

define RTL8188FU_OPENIPC_INSTALL_FIRMWARE
	$(INSTALL) -D -m 644 $(@D)/firmware/rtl8188fufw.bin \
		$(TARGET_DIR)/lib/firmware/rtlwifi/rtl8188fufw.bin
endef
RTL8188FU_OPENIPC_POST_INSTALL_TARGET_HOOKS += RTL8188FU_OPENIPC_INSTALL_FIRMWARE

$(eval $(kernel-module))
$(eval $(generic-package))
