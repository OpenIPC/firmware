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

# Buildroot's linuxptp installs the whole suite and its own SysV pair. This
# profile runs one daemon, ptp4l -S from S60precision-time, so the rest is
# dead weight on a board with a 5120KB squashfs -- and S65ptp4l/S66phc2sys
# are worse than dead: S65ptp4l starts a second ptp4l from
# /etc/linuxptp.cfg, which asks for hardware timestamping the FE-MAC does
# not have, and S66phc2sys starts the helper ptp4l.conf says is not needed.
# pmc stays as the one tool for asking ptp4l what it is doing. Removed
# at finalize because under per-package directories linuxptp's files only
# meet ours in the merged target.
define OPENIPC_PRECISION_TIME_PRUNE_LINUXPTP
	rm -f $(addprefix $(TARGET_DIR)/usr/sbin/,hwstamp_ctl nsm phc2sys \
		phc_ctl timemaster ts2phc tz2alt)
	rm -f $(TARGET_DIR)/etc/init.d/S65ptp4l $(TARGET_DIR)/etc/init.d/S66phc2sys
	rm -f $(TARGET_DIR)/etc/linuxptp.cfg
endef
OPENIPC_PRECISION_TIME_TARGET_FINALIZE_HOOKS += OPENIPC_PRECISION_TIME_PRUNE_LINUXPTP

$(eval $(generic-package))
