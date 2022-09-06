################################################################################
#
# ingenic-opensdk
#
################################################################################

INGENIC_OPENSDK_LICENSE = GPL-3.0
INGENIC_OPENSDK_LICENSE_FILES = LICENSE

FAMILY = $(shell grep "/board/" $(BR2_CONFIG) | head -1 | cut -d "/" -f 3)


define INGENIC_OPENSDK_EXTRACT_CMDS
	cp $(BR2_EXTERNAL_INGENIC_PATH)/package/ingenic-opensdk/src/kernel/* $(@D)/
endef

INGENIC_OPENSDK_MODULE_MAKE_OPTS = \
	CONFIG_GPIO=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR)

$(eval $(kernel-module))
$(eval $(generic-package))
