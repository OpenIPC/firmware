################################################################################
#
# ingenic-atbm603x-sdio-t31
#
################################################################################

INGENIC_ATBM603X_SDIO_T31_VERSION =
INGENIC_ATBM603X_SDIO_T31_SITE =
INGENIC_ATBM603X_SDIO_T31_LICENSE = GPL-2.0
INGENIC_ATBM603X_SDIO_T31_LICENSE_FILES = LICENSE

define INGENIC_ATBM603X_SDIO_T31_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/extra
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/atbm603x_conf
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/atbm603x_conf $(INGENIC_ATBM603X_SDIO_T31_PKGDIR)/files/*.txt

	$(if $(BR2_PACKAGE_INGENIC_ATBM603X_SDIO_T31_MMC0),\
		$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/extra $(INGENIC_ATBM603X_SDIO_T31_PKGDIR)/files/atbm603x_wifi_sdi0.ko)

	$(if $(BR2_PACKAGE_INGENIC_ATBM603X_SDIO_T31_MMC1),\
		$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.14__isvp_swan_1.0__/extra $(INGENIC_ATBM603X_SDIO_T31_PKGDIR)/files/atbm603x_wifi_sdi1.ko)
endef

$(eval $(generic-package))
