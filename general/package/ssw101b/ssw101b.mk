################################################################################
#
# ssw101b
#
################################################################################

ifeq ($(LOCAL_DOWNLOAD),y)
SSW101B_SITE_METHOD = git
SSW101B_SITE = https://github.com/openipc/ssw101b
SSW101B_VERSION = $(shell git ls-remote $(SSW101B_SITE) HEAD | head -1 | cut -f1)
else
SSW101B_SITE = https://github.com/openipc/ssw101b/archive
SSW101B_SOURCE = master.tar.gz
endif

SSW101B_LICENSE = GPL-2.0

SSW101B_MODULE_MAKE_OPTS = \
	CONFIG_SSW101B=y \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
