################################################################################
#
# openhisilicon
#
################################################################################

HISILICON_OPENSDK_VERSION = e8701ddf4f89f0d4a523ae92c4dc712aba356240
HISILICON_OPENSDK_SITE = $(call github,openipc,openhisilicon,$(HISILICON_OPENSDK_VERSION))
HISILICON_OPENSDK_LICENSE = GPL-3.0
HISILICON_OPENSDK_LICENSE_FILES = LICENSE

FAMILY = $(shell grep "/board/" $(BR2_CONFIG) | head -1 | cut -d "/" -f 3)

HISILICON_OPENSDK_MODULE_SUBDIRS = kernel
HISILICON_OPENSDK_MODULE_MAKE_OPTS = \
	DISABLE_IST=1 \
	DISABLE_PM=1 \
	DISABLE_TDE=1 \
	DISABLE_VO=1 \
	DISABLE_ISP=1 \
	CHIPARCH=$(FAMILY)

$(eval $(kernel-module))
$(eval $(generic-package))
