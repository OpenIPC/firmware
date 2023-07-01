################################################################################
#
# rtl8189fs-openipc
#
################################################################################

RTL8189FS_OPENIPC_SITE_METHOD = git
RTL8189FS_OPENIPC_SITE = https://github.com/jwrdegoede/rtl8189ES_linux
RTL8189FS_OPENIPC_VERSION = $(call EXTERNAL_SHA,$(RTL8189FS_OPENIPC_SITE),rtl8189fs)

RTL8189FS_OPENIPC_LICENSE = GPL-2.0
RTL8189FS_OPENIPC_LICENSE_FILES = COPYING

RTL8189FS_OPENIPC_MODULE_MAKE_OPTS = CONFIG_RTL8189FS=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
