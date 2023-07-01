################################################################################
#
# rtl8188eu
#
################################################################################

RTL8188EUS_OPENIPC_SITE_METHOD = git
RTL8188EUS_OPENIPC_SITE = https://github.com/aircrack-ng/rtl8188eus
RTL8188EUS_OPENIPC_VERSION = 6146193406b62e942d13d4d43580ed94ac70c218

RTL8188EUS_OPENIPC_LICENSE = GPL-2.0, proprietary (rtl8188fufw.bin firmware blob)
RTL8188EUS_OPENIPC_LICENSE_FILES = COPYING
RTL8188EUS_OPENIPC_MODULE_MAKE_OPTS = CONFIG_RTL8188EU=m CONFIG_CALIBRATE_TX_POWER_TO_MAX=y

define RTL8188EUS_OPENIPC_INSTALL_FIRMWARE
	$(INSTALL) -D -m 644 $(RTL8188EUS_OPENIPC_PKGDIR)/rtl8188fufw.bin \
		$(TARGET_DIR)/lib/firmware/rtlwifi/rtl8188fufw.bin
endef
RTL8188EUS_OPENIPC_POST_INSTALL_TARGET_HOOKS += RTL8188EUS_OPENIPC_INSTALL_FIRMWARE

$(eval $(kernel-module))
$(eval $(generic-package))
