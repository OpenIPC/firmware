################################################################################
#
# ax-agent | updated 2022.08.10
#
################################################################################

AX_AGENT_LICENSE = MIT
AX_AGENT_LICENSE_FILES = LICENSE

define AX_AGENT_EXTRACT_CMDS
	cp ../general/package/$(PKG_NAME)/src/$(PKG_NAME).c $(@D)/
endef

define AX_AGENT_BUILD_CMDS
	(cd $(@D); $(TARGET_CC) -s $(PKG_NAME).c -o ax-agent)
endef

define AX_AGENT_INSTALL_TARGET_CMDS
	install -m 0755 -D $(@D)/$(PKG_NAME) $(TARGET_DIR)/usr/sbin/$(PKG_NAME)
endef

$(eval $(generic-package))
