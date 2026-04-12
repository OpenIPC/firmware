################################################################################
#
# mini
#
################################################################################

MINI_SITE_METHOD = git
MINI_SITE = https://github.com/openipc/mini
MINI_VERSION = bc1e8b8e87a76b76399160f6b110d0d896037aa2

MINI_GIT_SUBMODULES = YES
MINI_LICENSE = GPL-3.0
MINI_LICENSE_FILES = LICENSE

ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516CV200),y)
	MINI_SDK_DIR = glutinium/hisi-osdrv2
else ifeq ($(BR2_PACKAGE_HISILICON_OSDRV_HI3516CV300),y)
	MINI_SDK_DIR = glutinium/hisi-osdrv3
endif

MINI_CONF_OPTS = -DPLATFORM_SDK_DIR=$(MINI_SDK_DIR)

define MINI_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 $(@D)/mini.ini $(TARGET_DIR)/etc/mini.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/mini
endef

$(eval $(cmake-package))
