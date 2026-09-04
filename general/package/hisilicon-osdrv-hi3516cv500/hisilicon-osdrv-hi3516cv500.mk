################################################################################
#
# hisilicon-osdrv-hi3516cv500
#
################################################################################

HISILICON_OSDRV_HI3516CV500_VERSION =
HISILICON_OSDRV_HI3516CV500_SITE =
HISILICON_OSDRV_HI3516CV500_LICENSE = MIT
HISILICON_OSDRV_HI3516CV500_LICENSE_FILES = LICENSE

# Kernel version from the actual build — matches opensdk.mk
HISILICON_OSDRV_HI3516CV500_KVER = $(shell cat $(BUILD_DIR)/linux-custom/include/config/kernel.release 2>/dev/null)
HISILICON_OSDRV_HI3516CV500_KMOD_DST = $(TARGET_DIR)/lib/modules/$(HISILICON_OSDRV_HI3516CV500_KVER)/hisilicon

# Common install: sensor configs, load scripts, vendor userspace libs
# When opensdk is enabled, .ko modules, sensor .so, and libisp.so come
# from opensdk instead — osdrv only provides what opensdk can't build.
#
# libsvpruntime.so (617 KB) and libnnie.so are deliberately not installed:
# nothing in any shipped cv500-family image links them (DT_NEEDED audit of
# the 2026-08-31 nightlies: zero consumers, zero dlopen string references),
# load_hisilicon's open_nnie lines are commented out, and libsvpruntime
# needs libstdc++, which rootfs_script.sh prunes — so the runtime was not
# even loadable on most builds. Together they cost ~215 KB of squashfs on
# every board of the family; hi3516av300_neo sat 12 KB from its cap because
# of it. An NNIE consumer that ever materialises puts them back here.

define HISILICON_OSDRV_HI3516CV500_INSTALL_COMMON
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/60fps
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/60fps $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/sensor/config/60fps/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/720p120fps
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/720p120fps $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/sensor/config/720p120fps/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/vga200fps
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/vga200fps $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/sensor/config/vga200fps/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/script/load*

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libaaccomm.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libaacdec.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libaacenc.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libaacsbrdec.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libaacsbrenc.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libdnvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/lib_hiacs.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/lib_hiae.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/lib_hiawb_natura.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/lib_hiawb.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/lib_hicalcflicker.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libhi_cipher.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/lib_hidehaze.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/lib_hidrc.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libhifisheyecalibrate.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/lib_hiir_auto.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/lib_hildci.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libive.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libmd.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libmpi.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libsecurec.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libtde.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libupvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/libVoiceEngine.so
	$(foreach lib,$(HISILICON_OSDRV_HI3516CV500_VQE_LIBS), \
		$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV500_PKGDIR)/files/lib/$(lib) ; \
	)
endef

# opensdk provides .ko modules, sensor .so, libisp.so. osdrv provides
# configs, load scripts, vendor-only userspace libs. The opensdk!=y
# branch that bundled vendor prebuilt .ko files was dropped — every
# modern OpenIPC build pairs osdrv with opensdk.
# Voice quality enhancement engines for the microphone capture path
# (OpenIPC/majestic#287). See hisilicon-osdrv-hi3516ev200.mk for the full
# reasoning: libupvqe.so is only the framework and dlopen()s one shared
# object per DSP stage when HI_MPI_AI_EnableVqe() runs, so without these that
# call fails with 0xa0158041 and the microphone is passed through unprocessed.
# AEC and EQ are not exposed by majestic, and the resampler lives inside
# libupvqe.so, so neither libhive_AEC/EQ.so nor libhive_RES{,_ext}.so is
# loaded.
#
# Ultimate only, matching the flavour majestic compiles VQE into. This family
# has no ultimate defconfig today (every hi3516cv500/av300/dv300 config is
# lite), so nothing changes for the images that exist — it is wired up so the
# libraries arrive with the first ultimate config rather than being missed.
ifeq ($(OPENIPC_MAJESTIC),ultimate)
HISILICON_OSDRV_HI3516CV500_VQE_LIBS = \
	libhive_common.so libhive_AGC.so libhive_ANR.so \
	libhive_HPF.so libhive_record.so
endif

define HISILICON_OSDRV_HI3516CV500_INSTALL_TARGET_CMDS
	$(call HISILICON_OSDRV_HI3516CV500_INSTALL_COMMON)
	$(INSTALL) -m 755 -d $(HISILICON_OSDRV_HI3516CV500_KMOD_DST)
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
endef

$(eval $(generic-package))
