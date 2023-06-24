################################################################################
#
# rtl8188fu
#
################################################################################

RTL8188FU_SITE_METHOD = git
RTL8188FU_SITE = https://github.com/viktorxda/realtek_wlan
RTL8188FU_VERSION = fe80762a68870d1c29111ea822d2be7f8fcdb935

RTL8188FU_LICENSE = GPL-2.0
RTL8188FU_LICENSE_FILES = COPYING

RTL8188FU_MODULE_MAKE_OPTS = \
	CONFIG_RTL8188FU=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
