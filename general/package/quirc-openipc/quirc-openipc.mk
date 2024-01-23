################################################################################
#
# quirc-openipc
#
################################################################################

QUIRC_OPENIPC_SITE = $(call github,openipc,quirc,$(QUIRC_OPENIPC_VERSION))
QUIRC_OPENIPC_VERSION = master

QUIRC_OPENIPC_DEPENDENCIES = libjpeg-openipc
QUIRC_OPENIPC_MAKE_OPTS = \
	CC=$(TARGET_CC) \
	AR=$(TARGET_AR)

define QUIRC_OPENIPC_BUILD_CMDS
	$(MAKE) $(QUIRC_OPENIPC_MAKE_OPTS) -C $(@D) all
endef

define QUIRC_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/qrscan
endef

$(eval $(generic-package))
