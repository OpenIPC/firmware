################################################################################
#
# logcat-openipc
#
# https://github.com/gtxaspec/linux_logcat
#
################################################################################

define LOGCAT_OPENIPC_EXTRACT_CMDS
        cp -av $(LOGCAT_OPENIPC_PKGDIR)/src/* $(@D)/
endef

LOGCAT_OPENIPC_INSTALL_STAGING = YES

$(eval $(cmake-package))
