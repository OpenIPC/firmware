################################################################################
#
# libjpeg-openipc
#
################################################################################

LIBJPEG_OPENIPC_VERSION = 9e
LIBJPEG_OPENIPC_SITE = http://www.ijg.org/files
LIBJPEG_OPENIPC_SOURCE = jpegsrc.v$(LIBJPEG_VERSION).tar.gz

LIBJPEG_OPENIPC_INSTALL_STAGING = YES
LIBJPEG_OPENIPC_LICENSE = IJG
LIBJPEG_OPENIPC_LICENSE_FILES = README

LIBJPEG_OPENIPC_CONF_OPTS = --disable-shared

define LIBJPEG_OPENIPC_REMOVE_BINARIES
	rm -f $(addprefix $(TARGET_DIR)/usr/bin/,cjpeg djpeg jpegtran rdjpgcom wrjpgcom)
endef

LIBJPEG_OPENIPC_POST_INSTALL_TARGET_HOOKS += LIBJPEG_OPENIPC_REMOVE_BINARIES

$(eval $(autotools-package))
$(eval $(host-autotools-package))
