################################################################################
#
# gdbserver-lite
#
################################################################################

GDBSERVER_LITE_SITE = $(call github,bet4it,gdbserver,$(GDBSERVER_LITE_VERSION))
GDBSERVER_LITE_VERSION = master

GDBSERVER_LITE_LICENSE = GNU General Public License v3.0
GDBSERVER_LITE_LICENSE_FILES = LICENSE

GDBSERVER_LITE_MAKE_OPTS = CROSS_COMPILE="$(TARGET_CROSS)" \
	CFLAGS="$(TARGET_CFLAGS)" \
	LDFLAGS="$(TARGET_LDFLAGS)" \
	HOSTCFLAGS="$(HOST_CFLAGS)" \
	STRIP=$(TARGET_STRIP) \
	OBJS=gdbserver.o utils.o packets.o signals.o

define GDBSERVER_LITE_BUILD_CMDS
	(cd $(@D); $(TARGET_CC) -Os -std=gnu99 -s gdbserver.c utils.c packets.c signals.c -o gdbserver)
endef

define GDBSERVER_LITE_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/gdbserver $(TARGET_DIR)/usr/sbin/gdbserver
endef

$(eval $(generic-package))
