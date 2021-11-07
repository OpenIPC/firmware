################################################################################
#
# hisilicon-opensdk | updated 2021.11.07
#
################################################################################

HISILICON_OPENSDK_VERSION = 164afc7dc6b322b3d3175d9eb5cfe1c81de3c00b
HISILICON_OPENSDK_SITE = $(call github,openipc,openhisilicon,$(MOTORS_VERSION))
HISILICON_OPENSDK_LICENSE = GPL-3.0
HISILICON_OPENSDK_LICENSE_FILES = LICENSE

define HISILICON_OPENSDK_BUILD_CMDS
	# (cd $(@D)/camhi-motor; $(TARGET_CC) -Os -s main.c -o camhi-motor)
endef

define HISILICON_OPENSDK_INSTALL_TARGET_CMDS
	# $(INSTALL) -m 0755 -D $(@D)/camhi-motor/camhi-motor $(TARGET_DIR)/usr/bin/camhi-motor
endef

$(eval $(generic-package))
