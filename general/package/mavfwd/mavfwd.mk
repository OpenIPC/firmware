################################################################################
#
# mavfwd
#
################################################################################

MAVFWD_SITE_METHOD = git
MAVFWD_SITE = https://github.com/openipc/mavfwd
MAVFWD_VERSION = c95613e241e03782fc6acc4780e60d8b4316a736

MAVFWD_LICENSE = MIT
MAVFWD_LICENSE_FILES = LICENSE

MAVFWD_DEPENDENCIES = libevent-openipc

define MAVFWD_BUILD_CMDS
    (cd $(@D); $(TARGET_CC) -levent_core -s mavfwd.c -o mavfwd)
endef

define MAVFWD_INSTALL_TARGET_CMDS
    $(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
    $(INSTALL) -m 0755 -D $(@D)/mavfwd $(TARGET_DIR)/usr/bin/mavfwd

    $(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
    cp $(MAVFWD_PKGDIR)/files/channels.sh $(TARGET_DIR)/usr/sbin
endef

$(eval $(generic-package))
