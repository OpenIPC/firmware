################################################################################
#
# vigil
#
################################################################################

VIGIL_SITE_METHOD = git
VIGIL_SITE = https://github.com/openipc/vigil
VIGIL_VERSION = 1dac27fca937d570df44c496372fe57645721c89
# LVGL is a submodule (third_party/lvgl).
VIGIL_GIT_SUBMODULES = YES
VIGIL_LICENSE = GPL-3.0-or-later
VIGIL_LICENSE_FILES = LICENSE

VIGIL_CONF_OPTS = -DBUILD_TESTING=OFF

ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3536DV100),y)
# Built against the SDK headers openhisilicon carries and the shared libmpi.so
# family hisilicon-osdrv-hi3536dv100 ships, both Hi3536DV100_SDK_V2.0.2.0, so
# vigild and the relinked modules agree on every ioctl.
VIGIL_DEPENDENCIES = hisilicon-opensdk hisilicon-osdrv-hi3536dv100
VIGIL_CONF_OPTS += \
	-DVIGIL_PLATFORM=hi3536dv100 \
	-DHI3536DV100_INCLUDE_DIR=$(HISILICON_OPENSDK_DIR)/kernel/include/hi3536dv100 \
	-DHI3536DV100_LIB_DIR=$(HISILICON_OSDRV_HI3536DV100_PKGDIR)/files/lib
endif

ifeq ($(BR2_PACKAGE_SIGMASTAR_OSDRV_INFINITY2M),y)
# The MI headers opensigmastar carries for the i2m NVR SDK libraries that
# sigmastar-osdrv-infinity2m ships.
VIGIL_DEPENDENCIES = sigmastar-opensdk sigmastar-osdrv-infinity2m
VIGIL_CONF_OPTS += \
	-DVIGIL_PLATFORM=sigmastar \
	-DSIGMASTAR_INCLUDE_DIR=$(SIGMASTAR_OPENSDK_DIR)/include/infinity2m \
	-DSIGMASTAR_LIB_DIR=$(SIGMASTAR_OSDRV_INFINITY2M_PKGDIR)/files/lib
endif

define VIGIL_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -D $(@D)/vigild $(TARGET_DIR)/usr/bin/vigild
	$(INSTALL) -m 755 -D $(VIGIL_PKGDIR)/files/S95vigil $(TARGET_DIR)/etc/init.d/S95vigil
	$(INSTALL) -m 644 -D $(VIGIL_PKGDIR)/files/vigil.yaml $(TARGET_DIR)/etc/vigil.yaml
endef

$(eval $(cmake-package))
