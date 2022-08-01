################################################################################
#
# hisilicon-opensdk | updated 2021.11.07
#
################################################################################

HISILICON_OPENSDK_VERSION = 7cc0bab2d40c0b35693f541cacd1bcb85d2c63b6
HISILICON_OPENSDK_SITE = $(call github,openipc,openhisilicon,$(HISILICON_OPENSDK_VERSION))
HISILICON_OPENSDK_LICENSE = GPL-2.0
HISILICON_OPENSDK_LICENSE_FILES = LICENSE

$(eval $(kernel-module))
$(eval $(generic-package))
