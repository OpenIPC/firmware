################################################################################
#
# rtl8733bu-openipc
#
################################################################################

RTL8733BU_OPENIPC_SITE = $(call github,openipc,realtek-wlan,$(RTL8733BU_OPENIPC_VERSION))
ifneq ($(findstring fpv,$(OPENIPC_VARIANT)),)
	RTL8733BU_OPENIPC_VERSION = rtl8733bu_fpv
else
	RTL8733BU_OPENIPC_VERSION = rtl8733bu
endif

RTL8733BU_OPENIPC_LICENSE = GPL-2.0
RTL8733BU_OPENIPC_LICENSE_FILES = COPYING

RTL8733BU_OPENIPC_MODULE_MAKE_OPTS = \
	CONFIG_RTL8733BU=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
