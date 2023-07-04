################################################################################
#
# rtl8812au-openipc
#
################################################################################

ifeq ($(LOCAL_DOWNLOAD),y)
RTL8812AU_OPENIPC_SITE_METHOD = git
RTL8812AU_OPENIPC_SITE = https://github.com/svpcom/rtl8812au
RTL8812AU_OPENIPC_VERSION = $(shell git ls-remote $(RTL8812AU_OPENIPC_SITE) HEAD | head -1 | cut -f1)
else
RTL8812AU_OPENIPC_SITE = https://github.com/svpcom/rtl8812au/archive
RTL8812AU_OPENIPC_SOURCE = master.tar.gz
endif

RTL8812AU_OPENIPC_LICENSE = GPL-2.0
RTL8812AU_OPENIPC_LICENSE_FILES = COPYING

RTL8812AU_OPENIPC_MODULE_MAKE_OPTS = CONFIG_RTL8812AU=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
