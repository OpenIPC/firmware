################################################################################
#
# hisilicon-opensdk
#
################################################################################

HISILICON_OPENSDK_SITE = $(call github,openipc,openhisilicon,$(HISILICON_OPENSDK_VERSION))
HISILICON_OPENSDK_VERSION = master

HISILICON_OPENSDK_LICENSE = GPL-3.0
HISILICON_OPENSDK_LICENSE_FILES = LICENSE

HISILICON_OPENSDK_MODULE_SUBDIRS = kernel
HISILICON_OPENSDK_MODULE_MAKE_OPTS = \
	DISABLE_IST=1 \
	DISABLE_PM=1 \
	DISABLE_TDE=1 \
	DISABLE_VO=1 \
	CHIPARCH=$(OPENIPC_SOC_FAMILY)

ifeq ($(OPENIPC_SOC_FAMILY),hi3516ev200)
	HISILICON_OPENSDK_MODULE_MAKE_OPTS += DISABLE_ISP=1
	HISILICON_OPENSDK_SDK_CODE = 0x3516E200
else ifeq ($(OPENIPC_SOC_FAMILY),gk7205v200)
	HISILICON_OPENSDK_SDK_CODE = 0x7205200
endif

# for userspace libraries
define HISILICON_OPENSDK_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) CHIPARCH=$(OPENIPC_SOC_FAMILY) SDK_CODE=$(HISILICON_OPENSDK_SDK_CODE) -C $(@D)/libraries all
endef

define HISILICON_OPENSDK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -D -m 0644 $(@D)/libraries/sensor/$(OPENIPC_SOC_FAMILY)/sony_imx335/libsns_imx335.so $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -D -m 0644 $(@D)/libraries/sensor/$(OPENIPC_SOC_FAMILY)/sony_imx307/libsns_imx307.so $(TARGET_DIR)/usr/lib/sensors
endef

$(eval $(kernel-module))
$(eval $(generic-package))
