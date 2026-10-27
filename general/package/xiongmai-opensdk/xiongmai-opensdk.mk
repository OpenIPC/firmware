################################################################################
#
# xiongmai-opensdk
#
################################################################################

XIONGMAI_OPENSDK_SITE = $(call github,openipc,openxiongmai,$(XIONGMAI_OPENSDK_VERSION))
XIONGMAI_OPENSDK_VERSION = 0097f2b93b1927eff9a7a5842b96445abcd8eab4

XIONGMAI_OPENSDK_LICENSE = GPL-3.0 (libraries), GPL-2.0 (xm_vi kernel module)
XIONGMAI_OPENSDK_LICENSE_FILES = LICENSE

# Install after osdrv, the same order hisilicon-opensdk keeps with its osdrv.
ifeq ($(BR2_PACKAGE_XIONGMAI_OSDRV_XM530),y)
XIONGMAI_OPENSDK_DEPENDENCIES += xiongmai-osdrv-xm530
endif

# The open VI driver (drivers/xm_vi): the vendor's xm530_vi.ko without its
# freeze at VI init or its vmalloc leak. The kernel-module infrastructure
# installs it as /lib/modules/<kver>/extra/xm_vi.ko, which load_xiongmai
# prefers over the vendor module.
XIONGMAI_OPENSDK_MODULE_SUBDIRS = drivers/xm_vi

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

$(eval $(kernel-module))
$(eval $(generic-package))
