################################################################################
#
# mavfwd
#
################################################################################

MAVFWD_SITE = $(call github,openipc,mavfwd,$(MAVFWD_VERSION))
MAVFWD_VERSION = HEAD

MAVFWD_LICENSE = MIT
MAVFWD_LICENSE_FILES = LICENSE

MAVFWD_DEPENDENCIES = libevent-openipc

define MAVFWD_BUILD_CMDS
	$(TARGET_CC) $(@D)/mavfwd.c -o $(@D)/mavfwd -levent_core -s
endef

define MAVFWD_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/mavfwd

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(MAVFWD_PKGDIR)/files/channels.sh
endef

$(eval $(generic-package))
