################################################################################
#
# rtl8812au-openipc
#
################################################################################

RTL8812AU_OPENIPC_SITE_METHOD = git
RTL8812AU_OPENIPC_SITE = https://github.com/svpcom/rtl8812au
RTL8812AU_OPENIPC_VERSION = $(call EXTERNAL_SHA,$(RTL8812AU_OPENIPC_SITE),HEAD)

RTL8812AU_OPENIPC_LICENSE = GPL-2.0
RTL8812AU_OPENIPC_LICENSE_FILES = COPYING

RTL8812AU_OPENIPC_MODULE_MAKE_OPTS = CONFIG_RTL8812AU=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
