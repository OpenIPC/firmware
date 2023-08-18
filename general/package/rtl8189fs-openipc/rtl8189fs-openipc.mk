################################################################################
#
# rtl8189fs-openipc
#
################################################################################

ifeq ($(LOCAL_DOWNLOAD),y)
RTL8189FS_OPENIPC_SITE_METHOD = git
RTL8189FS_OPENIPC_SITE = https://github.com/jwrdegoede/rtl8189es_linux
RTL8189FS_OPENIPC_VERSION = $(shell git ls-remote $(RTL8189FS_OPENIPC_SITE) rtl8189fs | head -1 | cut -f1)
else
RTL8189FS_OPENIPC_SITE = https://github.com/jwrdegoede/rtl8189es_linux/archive
RTL8189FS_OPENIPC_SOURCE = rtl8189fs.tar.gz
endif

RTL8189FS_OPENIPC_LICENSE = GPL-2.0
RTL8189FS_OPENIPC_LICENSE_FILES = COPYING

RTL8189FS_OPENIPC_MODULE_MAKE_OPTS = CONFIG_RTL8189FS=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

ifeq ($(BR2_PACKAGE_RTL8189FS_OPENIPC_INGENIC),y)
RTL8189FS_OPENIPC_PATCHES += 0002-ingenic_sdio.patch
endif

$(eval $(kernel-module))
$(eval $(generic-package))
