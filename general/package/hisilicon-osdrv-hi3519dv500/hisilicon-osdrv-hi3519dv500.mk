################################################################################
#
# hisilicon-osdrv-hi3519dv500
#
# Load script + vendor userspace MPP libraries for the Hi3519DV500 / Hi3516DV500
# (V5 "HISI_OT", aarch64 Cortex-A55) family. Mirrors hisilicon-osdrv-hi3516cv6xx.
#
# No vendor MPP userspace libraries ship, as on hi3516cv6xx: majestic links
# the gen 5 SDK (V1.0.1.1 B040, the release files/lib/ was taken from) from its
# static archives and carries only the code it reaches, and nothing else in
# the image links one -- the sensor drivers from hisilicon-opensdk declare only
# libc and take the ISP/AE/AWB callbacks and securec's functions from the
# majestic that dlopens them. As shared objects they were 41 files, 9.1 MB
# before compression, and with majestic beside them the 16 MB image overran
# its firmware partition by 1280 KB. files/lib/ is kept for a dynamically
# linked streamer that would want them.
#
################################################################################

HISILICON_OSDRV_HI3519DV500_VERSION =
HISILICON_OSDRV_HI3519DV500_SITE =
HISILICON_OSDRV_HI3519DV500_LICENSE = MIT
HISILICON_OSDRV_HI3519DV500_LICENSE_FILES = LICENSE

define HISILICON_OSDRV_HI3519DV500_INSTALL_TARGET_CMDS

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3519DV500_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3519DV500_PKGDIR)/files/script/set_allocator
endef

$(eval $(generic-package))
