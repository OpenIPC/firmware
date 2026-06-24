################################################################################
#
# goke-sns-bbi2c
#
# Software (GPIO bit-bang) i2c sensor bring-up for gk7205v200 boards that wire
# the image sensor to GPIO instead of the SoC hardware i2c controller.
#
################################################################################

GOKE_SNS_BBI2C_VERSION = 1.0
GOKE_SNS_BBI2C_SITE_METHOD = local
GOKE_SNS_BBI2C_SITE = $(GOKE_SNS_BBI2C_PKGDIR)/src
GOKE_SNS_BBI2C_LICENSE = GPL-2.0

define GOKE_SNS_BBI2C_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(GOKE_SNS_BBI2C_PKGDIR)/files/S96sensor-bbi2c \
		$(TARGET_DIR)/etc/init.d/S96sensor-bbi2c
endef

$(eval $(kernel-module))
$(eval $(generic-package))
