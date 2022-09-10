################################################################################
#
# hisilicon-opensdk
#
################################################################################

HISILICON_OPENSDK_VERSION = 347d018d23ee33c71e70db4f910a25b926aa070e
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
	CHIPARCH=$(FAMILY)

ifeq ($(FAMILY),hi3516ev200)
	HISILICON_OPENSDK_MODULE_MAKE_OPTS += DISABLE_ISP=1
endif

$(eval $(kernel-module))
$(eval $(generic-package))
