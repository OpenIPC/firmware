################################################################################
#
# wireless-configuration
#
################################################################################

WIRELESS_CONFIGURATION_VERSION =
WIRELESS_CONFIGURATION_SITE =
WIRELESS_CONFIGURATION_LICENSE = MIT
WIRELESS_CONFIGURATION_LICENSE_FILES = LICENSE

BR2_ROOTFS_POST_BUILD_SCRIPT += $(WIRELESS_CONFIGURATION_PKGDIR)/wireless-configuration.sh

$(eval $(generic-package))
