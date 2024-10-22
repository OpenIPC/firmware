################################################################################
#
# rtl8811cu-openipc
#
################################################################################

RTL8811CU_OPENIPC_SITE = $(call github,openipc,realtek-wlan,$(RTL8811CU_OPENIPC_VERSION))
RTL8811CU_OPENIPC_VERSION = rtl8811cu
RTL8811CU_OPENIPC_LICENSE = GPL-2.0
RTL8811CU_OPENIPC_LICENSE_FILES = COPYING

RTL8811CU_OPENIPC_MODULE_MAKE_OPTS = \
	CONFIG_RTL8821CU=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
