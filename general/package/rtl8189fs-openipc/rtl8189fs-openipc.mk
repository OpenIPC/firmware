################################################################################
#
# rtl8189fs-openipc
#
################################################################################

RTL8189FS_OPENIPC_SITE = $(call github,openipc,realtek-wlan,$(RTL8189FS_OPENIPC_VERSION))
RTL8189FS_OPENIPC_VERSION = rtl8189fs

RTL8189FS_OPENIPC_LICENSE = GPL-2.0
RTL8189FS_OPENIPC_LICENSE_FILES = COPYING

RTL8189FS_OPENIPC_MODULE_MAKE_OPTS = CONFIG_RTL8189FS=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
