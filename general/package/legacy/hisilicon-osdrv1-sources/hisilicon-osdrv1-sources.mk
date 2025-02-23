################################################################################
#
# hisilicon-osdrv1-sources package
#
################################################################################

HISILICON_OSDRV1_SOURCES_VERSION = 1.0
HISILICON_OSDRV1_SOURCES_SOURCE = mpp2-v1.tar.bz2
HISILICON_OSDRV1_SOURCES_SITE = https://github.com/SSharshunov/scrapyard/releases/download/some100
HISILICON_OSDRV1_SOURCES_SITE_METHOD = wget

define HISILICON_OSDRV1_SOURCES_BUILD_CMDS
	$(MAKE) -C $(@D)/mpp2-v1/mpp2/component/isp2/sensor/ all
endef

define HISILICON_OSDRV1_SOURCES_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(@D)/mpp2-v1/mpp2/component/isp2/lib/*.so
endef

$(eval $(generic-package))

