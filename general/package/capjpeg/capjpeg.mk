################################################################################
#
# capjpeg
#
################################################################################

CAPJPEG_SITE = $(call github,openipc,capjpeg,$(CAPJPEG_VERSION))
CAPJPEG_VERSION = HEAD

CAPJPEG_LICENSE = MIT
CAPJPEG_LICENSE_FILES = LICENSE

define CAPJPEG_INSTALL_TARGET_CMDS
$(INSTALL) -m 0755 -D $(@D)/capJPEG $(TARGET_DIR)/usr/bin/capjpeg
endef

$(eval $(generic-package))
