################################################################################
#
# ingenic-opensdk
#
################################################################################

INGENIC_OPENSDK_SITE = $(call github,openipc,openingenic,$(INGENIC_OPENSDK_VERSION))
INGENIC_OPENSDK_VERSION = master

INGENIC_OPENSDK_LICENSE = GPL-3.0
INGENIC_OPENSDK_LICENSE_FILES = LICENSE

INGENIC_OPENSDK_MODULE_SUBDIRS = kernel
INGENIC_OPENSDK_MODULE_MAKE_OPTS = \
	SOC=$(OPENIPC_SOC_MODEL) \
	SNS=$(OPENIPC_SNS_MODEL) \
	INSTALL_MOD_PATH=$(TARGET_DIR) \
	INSTALL_MOD_DIR=ingenic

$(eval $(kernel-module))
$(eval $(generic-package))
