################################################################################
#
# rtl8xxxu-openipc
#
################################################################################

# Local source tree (mainline rtl8xxxu v6.2 backported to 3.4.43).
RTL8XXXU_OPENIPC_VERSION = 1.0

RTL8XXXU_OPENIPC_SITE = $(BR2_EXTERNAL_GENERAL_PATH)/package/rtl8xxxu-openipc/src
RTL8XXXU_OPENIPC_SITE_METHOD = local

RTL8XXXU_OPENIPC_LICENSE = GPL-2.0-only
RTL8XXXU_OPENIPC_LICENSE_FILES = rtl8xxxu_core.c

RTL8XXXU_OPENIPC_MODULE_MAKE_OPTS = \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
