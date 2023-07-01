################################################################################
#
# ssw101b
#
################################################################################

SSW101B_SITE_METHOD = git
SSW101B_SITE = https://github.com/openipc/ssw101b
SSW101B_VERSION = $(call EXTERNAL_SHA,$(SSW101B_SITE),HEAD)

SSW101B_LICENSE = GPL-2.0

SSW101B_MODULE_MAKE_OPTS = \
	CONFIG_SSW101B=y \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
