################################################################################
#
# rtl8733bu-openipc
#
################################################################################

RTL8733BU_OPENIPC_VERSION = 8c35a579acbe67675c66021160ba8cc6522cd00c
RTL8733BU_OPENIPC_SITE = $(call github,gitguest0,rtl8733bu,$(RTL8733BU_OPENIPC_VERSION))
RTL8733BU_OPENIPC_LICENSE = GPL-2.0
RTL8733BU_OPENIPC_LICENSE_FILES = COPYING

RTL8733BU_OPENIPC_MODULE_MAKE_OPTS = \
	CONFIG_RTL8733BU=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
