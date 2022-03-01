################################################################################
#
# rtl8812au-openipc
#
################################################################################

RTL8812AU_OPENIPC_VERSION = 3a6402e9e79802891f1531b435be54f4d8b71f0b
RTL8812AU_OPENIPC_SITE = $(call github,aircrack-ng,rtl8812au,$(RTL8812AU_OPENIPC_VERSION))
RTL8812AU_OPENIPC_LICENSE = GPL-2.0
RTL8812AU_OPENIPC_LICENSE_FILES = COPYING

RTL8812AU_OPENIPC_MODULE_MAKE_OPTS = CONFIG_88XXAU=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
