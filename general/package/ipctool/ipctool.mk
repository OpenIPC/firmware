################################################################################
#
# ipctool
#
################################################################################

IPCTOOL_SITE_METHOD = git
IPCTOOL_SITE = http://github.com/openipc/ipctool
IPCTOOL_VERSION = $(shell git ls-remote $(IPCTOOL_SITE) HEAD | head -1 | awk '{ print $$1 }')

IPCTOOL_LICENSE = MIT
IPCTOOL_LICENSE_FILES = LICENSE

IPCTOOL_CONF_OPTS += -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release -DSKIP_VERSION=ON
IPCTOOL_MAKE_OPTS += VERBOSE=1

define IPCTOOL_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/ipcinfo
endef

$(eval $(cmake-package))
