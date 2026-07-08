################################################################################
#
# openipc-precision-time
#
# Virtual package: ships an opinionated ptp4l configuration and init script
# on top of the upstream linuxptp + chrony buildroot packages.
#
################################################################################

OPENIPC_PRECISION_TIME_VERSION = 1.0
OPENIPC_PRECISION_TIME_SITE_METHOD = local
OPENIPC_PRECISION_TIME_SITE = $(BR2_EXTERNAL_GENERAL_PATH)/package/openipc-precision-time
OPENIPC_PRECISION_TIME_LICENSE = MIT

define OPENIPC_PRECISION_TIME_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0644 $(@D)/files/ptp4l.conf $(TARGET_DIR)/etc/ptp4l.conf
	$(INSTALL) -D -m 0755 $(@D)/files/S60precision-time $(TARGET_DIR)/etc/init.d/S60precision-time
endef

$(eval $(generic-package))
