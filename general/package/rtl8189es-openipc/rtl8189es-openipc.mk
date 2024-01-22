################################################################################
#
# rtl8189es-openipc
#
################################################################################

RTL8189ES_OPENIPC_SITE = $(call github,jwrdegoede,rtl8189es_linux,$(RTL8189ES_OPENIPC_VERSION))
RTL8189ES_OPENIPC_VERSION = master

RTL8189ES_OPENIPC_LICENSE = GPL-2.0
RTL8189ES_OPENIPC_LICENSE_FILES = COPYING

RTL8189ES_OPENIPC_MODULE_MAKE_OPTS = CONFIG_RTL8189ES=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
