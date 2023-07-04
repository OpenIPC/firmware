################################################################################
#
# microsnander
#
################################################################################

ifeq ($(LOCAL_DOWNLOAD),y)
MICROSNANDER_SITE_METHOD = git
MICROSNANDER_SITE = https://github.com/openipc/microsnander
MICROSNANDER_VERSION = $(shell git ls-remote $(MICROSNANDER_SITE) HEAD | head -1 | cut -f1)
else
MICROSNANDER_SITE = https://github.com/openipc/microsnander/archive
MICROSNANDER_SOURCE = master.tar.gz
endif

MICROSNANDER_LICENSE = MIT
MICROSNANDER_LICENSE_FILES = LICENSE

define MICROSNANDER_BUILD_CMDS
	$(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)/src
endef

define MICROSNANDER_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/src/microsnander $(TARGET_DIR)/usr/sbin/microsnander
endef

$(eval $(generic-package))
