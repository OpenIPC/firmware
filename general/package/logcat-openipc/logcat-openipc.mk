################################################################################
#
# logcat-openipc
#
################################################################################

LOGCAT_OPENIPC_SITE = https://github.com/gtxaspec/linux_logcat/archive
LOGCAT_OPENIPC_SOURCE = master.tar.gz

LOGCAT_OPENIPC_LICENSE = GPL-2.0
LOGCAT_OPENIPC_LICENSE_FILES = COPYING

#define LOGCAT_OPENIPC_EXTRACT_CMDS
#        cp -av $(LOGCAT_OPENIPC_PKGDIR)/src/* $(@D)/
#endef


LOGCAT_OPENIPC_INSTALL_STAGING = YES

$(eval $(cmake-package))
