################################################################################
#
# rtl8733bu
#
################################################################################

ifeq ($(LOCAL_DOWNLOAD),y)
RTL8733BU_SITE_METHOD = git
RTL8733BU_SITE = https://github.com/viktorxda/realtek_wlan
RTL8733BU_VERSION = $(shell git ls-remote $(RTL8733BU_SITE) rtl8733bu | head -1 | cut -f1)
else
RTL8733BU_SITE = https://github.com/viktorxda/realtek_wlan/archive
RTL8733BU_SOURCE = rtl8733bu.tar.gz
endif

RTL8733BU_LICENSE = GPL-2.0
RTL8733BU_LICENSE_FILES = COPYING

RTL8733BU_MODULE_MAKE_OPTS = \
	CONFIG_RTL8733BU=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
