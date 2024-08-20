################################################################################
#
# datalink
#
################################################################################

DATALINK_VERSION =
DATALINK_SITE =
DATALINK_LICENSE = GPL-2.0

DATALINK_UNIT = drone
ifeq ($(OPENIPC_SOC_FAMILY),hi3536dv100)
	DATALINK_UNIT = gs
endif

define DATALINK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(DATALINK_PKGDIR)/files/datalink.conf
	$(INSTALL) -m 644 $(DATALINK_PKGDIR)/files/telemetry_$(DATALINK_UNIT).conf $(TARGET_DIR)/etc/telemetry.conf

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(DATALINK_PKGDIR)/files/S98datalink

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(DATALINK_PKGDIR)/files/telemetry
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(DATALINK_PKGDIR)/files/tweaksys
endef

$(eval $(generic-package))
