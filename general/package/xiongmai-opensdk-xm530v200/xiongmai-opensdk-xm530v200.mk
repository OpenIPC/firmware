################################################################################
#
# xiongmai-opensdk-xm530v200
#
################################################################################

XIONGMAI_OPENSDK_XM530V200_SITE = $(call github,openipc,openxiongmai,$(XIONGMAI_OPENSDK_XM530V200_VERSION))
XIONGMAI_OPENSDK_XM530V200_VERSION = 92f9f710bca956208b986aaec4959c75e17a3c84

XIONGMAI_OPENSDK_XM530V200_LICENSE = MIT (libxmv200), GPL-2.0 (open_mmz kernel module)
XIONGMAI_OPENSDK_XM530V200_LICENSE_FILES = libraries/libxmv200/LICENSE
XIONGMAI_OPENSDK_XM530V200_INSTALL_STAGING = YES

# The vendor modules it runs on, and load_xiongmai
XIONGMAI_OPENSDK_XM530V200_DEPENDENCIES = xiongmai-osdrv-xm530v200

# tools/xmv200-test builds libraries/libxmv200 first
define XIONGMAI_OPENSDK_XM530V200_BUILD_CMDS
	$(MAKE) CROSS_COMPILE=$(TARGET_CROSS) CFLAGS="$(TARGET_CFLAGS)" \
		-C $(@D)/tools/xmv200-test
endef

define XIONGMAI_OPENSDK_XM530V200_INSTALL_STAGING_CMDS
	$(INSTALL) -m 644 -D $(@D)/libraries/libxmv200/libxmv200.a $(STAGING_DIR)/usr/lib/libxmv200.a
	$(INSTALL) -m 644 -D $(@D)/libraries/libxmv200/xmv200.h $(STAGING_DIR)/usr/include/xmv200.h
endef

define XIONGMAI_OPENSDK_XM530V200_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -D $(@D)/tools/xmv200-test/xmv200-test $(TARGET_DIR)/usr/bin/xmv200-test
endef

# drivers/xm_mmz (open_mmz.ko) with the V200 series' allocator, which
# load_xiongmai prefers over the vendor mmz.ko. The kernel-module
# infrastructure installs it under /lib/modules/<kver>/extra/;
# xiongmai-osdrv-xm530v200 then leaves out the vendor mmz.ko.
XIONGMAI_OPENSDK_XM530V200_MODULE_SUBDIRS = drivers/xm_mmz
XIONGMAI_OPENSDK_XM530V200_MODULE_MAKE_OPTS = MMZ_ALLOCATOR=list

$(eval $(kernel-module))
$(eval $(generic-package))
