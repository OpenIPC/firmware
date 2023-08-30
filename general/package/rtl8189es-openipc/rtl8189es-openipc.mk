################################################################################
#
# rtl8189es-openipc
#
################################################################################

ifeq ($(LOCAL_DOWNLOAD),y)
RTL8189ES_OPENIPC_SITE_METHOD = git
RTL8189ES_OPENIPC_SITE = https://github.com/jwrdegoede/rtl8189es_linux
RTL8189ES_OPENIPC_VERSION = $(shell git ls-remote $(RTL8189ES_OPENIPC_SITE) master | head -1 | cut -f1)
else
RTL8189ES_OPENIPC_SITE = https://github.com/jwrdegoede/rtl8189es_linux/archive
RTL8189ES_OPENIPC_SOURCE = master.tar.gz
endif

RTL8189ES_OPENIPC_LICENSE = GPL-2.0
RTL8189ES_OPENIPC_LICENSE_FILES = COPYING

RTL8189ES_OPENIPC_MODULE_MAKE_OPTS = CONFIG_RTL8189ES=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
