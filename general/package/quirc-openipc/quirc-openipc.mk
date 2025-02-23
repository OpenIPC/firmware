################################################################################
#
# quirc-openipc
#
################################################################################

QUIRC_OPENIPC_SITE = $(call github,openipc,quirc,$(QUIRC_OPENIPC_VERSION))
QUIRC_OPENIPC_VERSION = HEAD

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

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/sbin $(QUIRC_OPENIPC_PKGDIR)/files/qrscan.sh

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(QUIRC_OPENIPC_PKGDIR)/files/S97qrscan

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/openipc/sounds
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/openipc/sounds $(QUIRC_OPENIPC_PKGDIR)/files/ready_48k.pcm
endef

$(eval $(generic-package))
