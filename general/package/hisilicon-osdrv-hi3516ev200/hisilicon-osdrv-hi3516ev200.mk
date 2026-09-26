################################################################################
#
# hisilicon-osdrv-hi3516ev200
#
################################################################################

HISILICON_OSDRV_HI3516EV200_VERSION =
HISILICON_OSDRV_HI3516EV200_SITE =
HISILICON_OSDRV_HI3516EV200_LICENSE = MIT
HISILICON_OSDRV_HI3516EV200_LICENSE_FILES = LICENSE

# Voice quality enhancement engines for the microphone capture path
# (OpenIPC/majestic#287). libupvqe.so is only the framework: it dlopen()s one
# shared object per DSP stage when HI_MPI_AI_EnableVqe() runs, so an image
# without these answers that call with 0xa0158041 and
# "dlopen ... libhive_HPF.so failed", and the microphone is passed through
# unprocessed however the attributes were set.
#
# Only the stages majestic can switch on: the high-pass filter, noise
# reduction and automatic gain control that make up the 8/16 kHz talk engine,
# plus the 48 kHz record engine. libhive_AEC.so (echo cancellation) and
# libhive_EQ.so are not exposed, and the resampler is compiled into
# libupvqe.so itself (RES_ReSampler_*), so libhive_RES.so is never loaded.
# libhive_common.so is not a stage but is a NEEDED of AGC and ANR.
#
# Ultimate only, matching the flavour majestic compiles VQE into at all:
# 268 KB that nothing in a lite image would ever dlopen().
ifeq ($(OPENIPC_MAJESTIC),ultimate)
HISILICON_OSDRV_HI3516EV200_VQE_LIBS = \
	libhive_common.so libhive_AGC.so libhive_ANR.so \
	libhive_HPF.so libhive_record.so
endif

# Which IMX335 mode is the default. majestic, given no isp.sensorConfig, takes
# the one profile in /etc/sensors whose name starts with the detected sensor
# (imx335_i2c), so the file names are the default.
#
# The 128 MB parts get the full 5 MP mode: imx335_i2c_5M.ini is the one that
# matches, and the old 5M name stays as a link for configs that spell it out
# (it cannot match, since it does not start with imx335). Every other part in
# this family keeps the old names and so the 4 MP mode -- a 5 MP frame pool
# does not fit their 64 MB. The old 4M name is removed on the 128 MB parts, or
# a rebuild over an earlier target would leave two profiles matching.
HISILICON_OSDRV_HI3516EV200_IMX335_5M_SOCS = hi3516ev300 hi3516dv200 gk7205v300 gk7605v100

ifneq ($(filter $(OPENIPC_SOC_MODEL),$(HISILICON_OSDRV_HI3516EV200_IMX335_5M_SOCS)),)
define HISILICON_OSDRV_HI3516EV200_IMX335_LAYOUT
	for d in $(TARGET_DIR)/etc/sensors $(TARGET_DIR)/etc/sensors/WDR; do \
		rm -f $$d/imx335_i2c_4M.ini; \
		ln -sf imx335_i2c_5M.ini $$d/5M_imx335.ini; \
	done
endef
else
define HISILICON_OSDRV_HI3516EV200_IMX335_LAYOUT
	for d in $(TARGET_DIR)/etc/sensors $(TARGET_DIR)/etc/sensors/WDR; do \
		mv -f $$d/4M_imx335_i2c.ini $$d/imx335_i2c_4M.ini; \
		mv -f $$d/imx335_i2c_5M.ini $$d/5M_imx335.ini; \
	done
endef
endif

define HISILICON_OSDRV_HI3516EV200_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/sensor/config/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/WDR
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/WDR $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/sensor/config/WDR/*.ini
	$(HISILICON_OSDRV_HI3516EV200_IMX335_LAYOUT)

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/high-fps
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/high-fps $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/sensor/config/high-fps/*.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/iq
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/sensor/iq/imx307.ini
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/sensor/iq/imx335.ini
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/sensor/iq/f23.ini
	ln -sf imx307.ini $(TARGET_DIR)/etc/sensors/iq/default.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/4.9.37/hisilicon
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/4.9.37/hisilicon $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/kmod/camhi-motor.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/script/ircut_demo
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/script/set_allocator

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/sensor/*.so

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libaaccomm.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libaacdec.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libaacenc.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libaacsbrdec.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libaacsbrenc.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libbcd.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libdnvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/lib_hiae.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/lib_hiawb_natura.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/lib_hiawb.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libhi_cipher.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/lib_hidehaze.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/lib_hidrc.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/lib_hiir_auto.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/lib_hildci.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libisp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libive.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libivp.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libmd.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libmpi.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libsecurec.so
	# $(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libtde.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libupvqe.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/libVoiceEngine.so
	$(foreach lib,$(HISILICON_OSDRV_HI3516EV200_VQE_LIBS), \
		$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/ $(HISILICON_OSDRV_HI3516EV200_PKGDIR)/files/lib/$(lib) ; \
	)
endef

$(eval $(generic-package))
