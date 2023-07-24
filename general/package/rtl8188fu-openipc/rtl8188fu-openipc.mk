################################################################################
#
# rtl8188fu-openipc
#
################################################################################

RTL8188FU_OPENIPC_SITE_METHOD = git
RTL8188FU_OPENIPC_SITE = https://github.com/openipc/realtek-wlan
RTL8188FU_OPENIPC_VERSION = $(shell git ls-remote $(RTL8188FU_OPENIPC_SITE) rtl8188fu | head -1 | cut -f1)

RTL8188FU_OPENIPC_LICENSE = GPL-2.0
RTL8188FU_OPENIPC_LICENSE_FILES = COPYING

RTL8188FU_OPENIPC_MODULE_MAKE_OPTS = \
	CONFIG_RTL8188FU=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
