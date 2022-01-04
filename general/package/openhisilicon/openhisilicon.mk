################################################################################
#
# openhisilicon
#
################################################################################

#OPENHISILICON_VERSION = HEAD
#OPENHISILICON_SITE = $(call github,openipc,openhisilicon,$(OPENHISILICON_VERSION))
OPENHISILICON_LICENSE = GPL-2.0
OPENHISILICON_LICENSE_FILES = COPYING

OPENHISILICON_MODULE_MAKE_OPTS = \
	CONFIG_HWRNG=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

define OPENHISILICON_EXTRACT_CMDS
	cp -v $(BR2_EXTERNAL_HISILICON_PATH)/package/openhisilicon/src/* $(@D)/
endef



$(eval $(kernel-module))
$(eval $(generic-package))
