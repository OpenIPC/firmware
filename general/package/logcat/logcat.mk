################################################################################
#
# logcat
# https://github.com/wltechblog/logcat-mini
#
################################################################################

LOGCAT_SITE_METHOD = local
LOGCAT_SITE = $(LOGCAT_PKGDIR)/src
LOGCAT_LICENSE = MIT
LOGCAT_LICENSE_FILES = LICENSE

define LOGCAT_BUILD_CMDS
	$(TARGET_CC) $(@D)/logcat.c -o $(@D)/logcat -s
endef

define LOGCAT_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/logcat
endef

$(eval $(generic-package))
