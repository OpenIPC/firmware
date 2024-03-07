################################################################################
#
# logcat-openipc
#
################################################################################

LOGCAT_OPENIPC_SITE = $(call github,gtxaspec,linux_logcat,$(LOGCAT_OPENIPC_VERSION))
LOGCAT_OPENIPC_VERSION = HEAD

LOGCAT_OPENIPC_LICENSE = GPL-2.0
LOGCAT_OPENIPC_LICENSE_FILES = COPYING

LOGCAT_OPENIPC_INSTALL_STAGING = YES

$(eval $(cmake-package))
