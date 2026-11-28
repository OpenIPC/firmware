################################################################################
#
# ingenic-osdrv-t23-v4l2
#
################################################################################

INGENIC_OSDRV_T23_V4L2_VERSION =
INGENIC_OSDRV_T23_V4L2_SITE =
INGENIC_OSDRV_T23_V4L2_LICENSE = MIT

define INGENIC_OSDRV_T23_V4L2_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 755 $(INGENIC_OSDRV_T23_V4L2_PKGDIR)/files/script/load_ingenic \
		$(TARGET_DIR)/usr/bin/load_ingenic
endef

$(eval $(generic-package))
