################################################################################
#
# comgt | updated 2022.09.13
#
################################################################################

COMGT_LICENSE = GPL
COMGT_LICENSE_FILES = LICENSE

define COMGT_EXTRACT_CMDS
	cp -a ../general/package/comgt/* $(@D)/
endef

define COMGT_BUILD_CMDS
	$(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)/src
endef

define COMGT_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	install -m 0755 -D $(@D)/src/comgt $(TARGET_DIR)/usr/sbin/comgt
endef

$(eval $(generic-package))
