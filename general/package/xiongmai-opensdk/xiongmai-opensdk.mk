################################################################################
#
# xiongmai-opensdk
#
################################################################################

XIONGMAI_OPENSDK_SITE = $(call github,openipc,openxiongmai,$(XIONGMAI_OPENSDK_VERSION))
XIONGMAI_OPENSDK_VERSION = ce1068c5f2a3a48bc3d0905563909b99090df11c

XIONGMAI_OPENSDK_LICENSE = GPL-3.0 (libraries), GPL-2.0 (open_vi, open_mmz, open_isp, open_rgn kernel modules)
XIONGMAI_OPENSDK_LICENSE_FILES = LICENSE

# Install after osdrv, the same order hisilicon-opensdk keeps with its osdrv.
ifeq ($(BR2_PACKAGE_XIONGMAI_OSDRV_XM530),y)
XIONGMAI_OPENSDK_DEPENDENCIES += xiongmai-osdrv-xm530
endif

# The open drivers, which load_xiongmai prefers over the vendor modules:
# drivers/xm_vi (open_vi.ko), the vendor's xm530_vi.ko without its freeze at
# VI init or its vmalloc leak, drivers/xm_mmz (open_mmz.ko), its mmz.ko, and
# drivers/xm530_isp (open_isp.ko), its xm530_isp.ko, and drivers/xm530_rgn
# (open_rgn.ko), its xm530_rgn.ko. The kernel-module infrastructure installs
# them under /lib/modules/<kver>/extra/; xiongmai-osdrv-xm530 then leaves out
# the vendor mmz.ko.
XIONGMAI_OPENSDK_MODULE_SUBDIRS = drivers/xm_vi drivers/xm_mmz drivers/xm530_isp drivers/xm530_rgn

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
