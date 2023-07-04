################################################################################
#
# rtl8733bu
#
################################################################################

RTL8733BU_SITE_METHOD = git
RTL8733BU_SITE = https://github.com/viktorxda/realtek_wlan
RTL8733BU_VERSION = $(call EXTERNAL_SHA,$(RTL8733BU_SITE),rtl8733bu)

RTL8733BU_LICENSE = GPL-2.0
RTL8733BU_LICENSE_FILES = COPYING

RTL8733BU_MODULE_MAKE_OPTS = \
	CONFIG_RTL8733BU=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
