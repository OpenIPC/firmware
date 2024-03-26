################################################################################
#
# logcat-openipc
#
################################################################################

LOGCAT_OPENIPC_SITE_METHOD = local
LOGCAT_OPENIPC_SITE = $(LOGCAT_OPENIPC_PKGDIR)/src

define LOGCAT_OPENIPC_BUILD_CMDS
	$(TARGET_CC) $(@D)/logcat.c -o $(@D)/logcat -s
endef

define LOGCAT_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/logcat
endef

$(eval $(generic-package))
