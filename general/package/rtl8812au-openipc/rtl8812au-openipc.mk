################################################################################
#
# rtl8812au-openipc
#
################################################################################

RTL8812AU_OPENIPC_SITE = $(call github,svpcom,rtl8812au,$(RTL8812AU_OPENIPC_VERSION))
RTL8812AU_OPENIPC_VERSION = dc9993dd6e7de738de8437add043747a6bfc475c

RTL8812AU_OPENIPC_LICENSE = GPL-2.0
RTL8812AU_OPENIPC_LICENSE_FILES = COPYING

RTL8812AU_OPENIPC_MODULE_MAKE_OPTS = CONFIG_RTL8812AU=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
