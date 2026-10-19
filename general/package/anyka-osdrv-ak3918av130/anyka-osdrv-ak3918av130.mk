################################################################################
#
# anyka-osdrv-ak3918av130
#
################################################################################

ANYKA_OSDRV_AK3918AV130_VERSION =
ANYKA_OSDRV_AK3918AV130_SITE =
ANYKA_OSDRV_AK3918AV130_LICENSE = PROPRIETARY

# files/kmod is a subset of driver/external/ from os/driver_normal.tar.gz
# in the AnyCloud39AV130 SDK, unmodified:
#   https://github.com/Link-WQS/ANYKA_130L_SDK
#   commit 3480d024b682ccb2cf8bcb87dc4d2ac0af4dc49e
# The SDK carries no source for the ISP, encoder, audio, Ethernet or SD
# drivers. It does for the sensors (os/driver_src/sensor), which could move to
# a from-source package like the other vendors' opensdk.
#
# The SDK builds these against its own 4.4.302-cip94 tree. The family's
# kernel (OpenIPC/linux anyka-ak3918ev300) is mainline 4.4.302 with the
# AV130 machine imported and the structures these modules share laid out the
# vendor's way; board/ak3918ev300/ak3918av130's CONFIG_LOCALVERSION="-cip94"
# gives it the release string and vermagic they were built for. That is a
# vermagic match, not a CIP kernel.
ANYKA_OSDRV_AK3918AV130_KVER = 4.4.302-cip94

define ANYKA_OSDRV_AK3918AV130_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/$(ANYKA_OSDRV_AK3918AV130_KVER)/anyka
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/$(ANYKA_OSDRV_AK3918AV130_KVER)/anyka $(ANYKA_OSDRV_AK3918AV130_PKGDIR)/files/kmod/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(ANYKA_OSDRV_AK3918AV130_PKGDIR)/files/script/load_anyka
endef

$(eval $(generic-package))
