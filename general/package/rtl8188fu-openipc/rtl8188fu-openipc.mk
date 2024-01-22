################################################################################
#
# rtl8188fu-openipc
#
################################################################################

RTL8188FU_OPENIPC_SITE = $(call github,openipc,realtek-wlan,$(RTL8188FU_OPENIPC_VERSION))
RTL8188FU_OPENIPC_VERSION = rtl8188fu

RTL8188FU_OPENIPC_LICENSE = GPL-2.0
RTL8188FU_OPENIPC_LICENSE_FILES = COPYING

RTL8188FU_OPENIPC_MODULE_MAKE_OPTS = \
	CONFIG_RTL8188FU=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
