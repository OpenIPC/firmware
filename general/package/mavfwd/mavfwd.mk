################################################################################
#
# mavfwd
#
################################################################################

# MAVFWD_VERSION = $(shell git ls-remote https://github.com/OpenIPC/mavfwd/ master | awk '{ print $$1 }')
MAVFWD_VERSION = 220d30e118d26008e94445887a03d77ba73c2d29
MAVFWD_SITE = $(call github,OpenIPC,mavfwd,$(MAVFWD_VERSION))
MAVFWD_LICENSE = MIT
MAVFWD_LICENSE_FILES = LICENSE

MAVFWD_DEPENDENCIES = libevent-openipc

define MAVFWD_BUILD_CMDS
    (cd $(@D); $(TARGET_CC) -levent_core -s mavfwd.c -o mavfwd)
endef

define MAVFWD_INSTALL_TARGET_CMDS
    $(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
    $(INSTALL) -m 0755 -D $(@D)/mavfwd $(TARGET_DIR)/usr/sbin/mavfwd
endef

$(eval $(generic-package))
