################################################################################
#
# xiongmai-opensdk
#
################################################################################

XIONGMAI_OPENSDK_SITE = $(call github,openipc,openxiongmai,$(XIONGMAI_OPENSDK_VERSION))
XIONGMAI_OPENSDK_VERSION = cdc61345e53c7655bfb70df1c4cb47e9edf938bc

XIONGMAI_OPENSDK_LICENSE = GPL-3.0
XIONGMAI_OPENSDK_LICENSE_FILES = LICENSE

# Install after osdrv, the same order hisilicon-opensdk keeps with its osdrv.
ifeq ($(BR2_PACKAGE_XIONGMAI_OSDRV_XM530),y)
XIONGMAI_OPENSDK_DEPENDENCIES += xiongmai-osdrv-xm530
endif

define XIONGMAI_OPENSDK_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) CHIPARCH=$(OPENIPC_SOC_FAMILY) -C $(@D)/libraries all
endef

# load_xiongmai links /usr/lib/libsns.so to one of the sensor libraries,
# picked by the sensor ipcinfo reports.
define XIONGMAI_OPENSDK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors \
		$(@D)/libraries/libsns_X50_XM530/libsns_X50_XM530.so \
		$(@D)/libraries/libsns_X123_XM530/libsns_X123_XM530.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib \
		$(@D)/libraries/libispapp_XM530/libispapp_XM530.so
endef

$(eval $(generic-package))
