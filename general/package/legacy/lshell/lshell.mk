################################################################################
#
# lshell | updated 2024.07.13
#
################################################################################

LSHELL_LICENSE = GPL
LSHELL_LICENSE_FILES = LICENSE

define LSHELL_EXTRACT_CMDS
	cp -a $(LSHELL_PKGDIR)/* $(@D)/
endef

define LSHELL_BUILD_CMDS
	$(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D)/src
endef

define LSHELL_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/bin
	$(INSTALL) -m 755 -D $(@D)/src/lshell $(TARGET_DIR)/bin/lshell
endef

$(eval $(generic-package))
