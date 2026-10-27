################################################################################
#
# sigmastar-osdrv-msc313e
#
################################################################################

SIGMASTAR_OSDRV_MSC313E_VERSION =
SIGMASTAR_OSDRV_MSC313E_SITE =
SIGMASTAR_OSDRV_MSC313E_LICENSE = MIT
SIGMASTAR_OSDRV_MSC313E_LICENSE_FILES = LICENSE

# The vendor libs are glibc builds; on the musl rootfs glibc-compat supplies
# the loader names and the glibc-only symbols they import.
SIGMASTAR_OSDRV_MSC313E_DEPENDENCIES = glibc-compat

# The OMX core (libOMX_BELA) dlopens its components as /lib/<name>, or from
# $OMX_LIB_PATH when that is set; it never searches the library path. Keep
# them in /lib so nothing has to set it.
SIGMASTAR_OSDRV_MSC313E_OMX_COMPONENTS = $(addprefix \
	$(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/lib/libOMX_,$(addsuffix .so, \
	ACODEC ALSA ASPL AVQE_A IJPE VMFE VSPL VSRC VVHE))

# libmi and libOMX_BELA dlopen("libat.so") for property_get/property_set and
# libCam* link it, so a library of that name has to exist. The vendor one is
# Android's property client and needs the vendor's Android init (minit) to
# serve it; this one reads /etc/sigmastar.conf instead.
define SIGMASTAR_OSDRV_MSC313E_BUILD_CMDS
	$(TARGET_CC) $(TARGET_CFLAGS) -fPIC -shared -Wl,-soname,libat.so \
		$(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/src/libat.c -o $(@D)/libat.so -lpthread -s
	$(TARGET_CC) $(TARGET_CFLAGS) $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/src/dmem.c \
		-o $(@D)/dmem -s
endef

define SIGMASTAR_OSDRV_MSC313E_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/iqfile/audio
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/iqfile/sensor_db.cfg
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/iqfile/*.xml
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/iqfile/f22*.mz
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/iqfile/imx307*.mz
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/iqfile/imx323*.mz
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/iqfile/sc2235*.mz
#	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/iqfile/*.mz
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iqfile/audio $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/iqfile/audio/*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.18.30/sigmastar
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.18.30/sigmastar $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/kmod/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/script/ircut_demo
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/bin/*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/dmem

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(filter-out $(SIGMASTAR_OSDRV_MSC313E_OMX_COMPONENTS), \
		$(wildcard $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/lib/*.so))
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/ $(SIGMASTAR_OSDRV_MSC313E_OMX_COMPONENTS)
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(@D)/libat.so
	# libOMX_AVQE_A moves the playback DMA pointer with a 32-bit-off_t lseek,
	# which musl's 64-bit one would misread; send it to glibc-compat's wrapper.
	python3 $(GLIBC_COMPAT_PKGDIR)/rename-import.py \
		$(TARGET_DIR)/lib/libOMX_AVQE_A.so lseek lse32
	$(INSTALL) -m 644 -D $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sigmastar.conf $(TARGET_DIR)/etc/sigmastar.conf

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/*JXF22*.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/*imx307*.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/*imx323*.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/*sc2235*.so
#	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(SIGMASTAR_OSDRV_MSC313E_PKGDIR)/files/sensor/*.so

endef

$(eval $(generic-package))
