################################################################################
#
# ipctool
#
################################################################################

IPCTOOL_VERSION = $(shell git ls-remote https://github.com/OpenIPC/ipctool/ master | awk '{ print $$1 }')
IPCTOOL_SITE = https://github.com/OpenIPC/ipctool.git
IPCTOOL_SITE_METHOD = git
IPCTOOL_LICENSE = MIT
IPCTOOL_LICENSE_FILES = LICENSE

IPCTOOL_CONF_OPTS += -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release

IPCTOOL_MAKE_OPTS += VERBOSE=1

define IPCTOOL_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/ipcinfo
endef

$(eval $(cmake-package))
