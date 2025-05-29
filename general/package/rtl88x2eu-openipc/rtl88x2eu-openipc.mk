################################################################################
#
# rtl88x2eu-openipc
#
################################################################################

RTL88X2EU_OPENIPC_SITE = $(call github,libc0607,rtl88x2eu-20230815,$(RTL88X2EU_OPENIPC_VERSION))
RTL88X2EU_OPENIPC_VERSION = HEAD

RTL88X2EU_OPENIPC_LICENSE = GPL-2.0
RTL88X2EU_OPENIPC_LICENSE_FILES = COPYING

RTL88X2EU_OPENIPC_MODULE_MAKE_OPTS = CONFIG_RTL8822EU=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
