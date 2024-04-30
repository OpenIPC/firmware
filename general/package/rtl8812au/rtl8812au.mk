################################################################################
#
# rtl8812au
#
################################################################################

RTL8812AU_SITE = $(call github,openipc,realtek-wlan,$(RTL8812AU_VERSION))
RTL8812AU_VERSION = rtl8812au

RTL8812AU_MODULE_MAKE_OPTS = \
	CONFIG_RTL8812AU=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
