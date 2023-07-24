################################################################################
#
# rtl8733bu-openipc
#
################################################################################

RTL8733BU_OPENIPC_SITE_METHOD = git
RTL8733BU_OPENIPC_SITE = https://github.com/openipc/realtek-wlan
RTL8733BU_OPENIPC_VERSION = $(shell git ls-remote $(RTL8733BU_OPENIPC_SITE) rtl8733bu | head -1 | cut -f1)

RTL8733BU_OPENIPC_LICENSE = GPL-2.0
RTL8733BU_OPENIPC_LICENSE_FILES = COPYING

RTL8733BU_OPENIPC_MODULE_MAKE_OPTS = \
	CONFIG_RTL8733BU=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
