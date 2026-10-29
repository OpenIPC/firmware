################################################################################
#
# hisilicon-osdrv-hi3516cv6xx
#
# Load script + vendor userspace MPP libraries for the Hi3516CV6xx (V5)
# family. Kernel modules and sensor drivers are no longer shipped here:
#  * kernel modules now built against the running openipc/linux tree by
#    hisilicon-opensdk (see ../hisilicon-opensdk/hisilicon-opensdk.mk).
#    Shipping prebuilt vendor .ko caused init-time hangs because struct
#    module / vermagic / config layouts didn't match openipc's kernel.
#  * sensor drivers now built from vendor SDK source by hisilicon-opensdk
#    via libraries/sensor/hi3516cv6xx/ and HISILICON_OPENSDK_SENSORS_hi3516cv6xx.
#
# Vendor MPP userspace .so blobs ship from files/lib/ — no openhisilicon
# V5 source mirror exists. The install list below is the transitive
# NEEDED closure of the majestic binary against the vendor lib set
# (computed via readelf -d), with libopus.so excluded because
# BR2_PACKAGE_OPUS_OPENIPC + BR2_PACKAGE_OPUS_OPENIPC_HISI_SHIM build
# an open equivalent that covers both the standard opus_* API and the
# 6 HiSi-only ot_opus_* extensions used by libss_mpi_audio_adp.so (the
# shim source lives in general/package/opus-openipc/src/ot_opus_shim.c
# and is baked into libopus.so by the package's gated POST_EXTRACT
# hook). The 13 libs not in the closure (libss_mpi_aibnr/bla/cipher/
# devstat/km/otp/smartae/syskol/uvc, libss_bcd,
# libmbedtls_harden_adapt, libsvp_aicpu, libvqe_common) save ~660 KB
# and are intentionally omitted.
#
################################################################################

HISILICON_OSDRV_HI3516CV6XX_VERSION =
HISILICON_OSDRV_HI3516CV6XX_SITE =
HISILICON_OSDRV_HI3516CV6XX_LICENSE = MIT
HISILICON_OSDRV_HI3516CV6XX_LICENSE_FILES = LICENSE

# Vendor MPP userspace .so files needed by majestic (transitive NEEDED
# closure). Excludes libopus.so (provided by BR2_PACKAGE_OPUS_OPENIPC).
HISILICON_OSDRV_HI3516CV6XX_MPP_LIBS = \
	libaac_comm.so libaac_dec.so libaac_enc.so \
	libaac_sbr_dec.so libaac_sbr_enc.so \
	libacs.so libaiisp.so libbnr.so libcalcflicker.so \
	libdehaze.so libdnvqe.so libdrc.so \
	libextend_stats.so libir_auto.so libldci.so \
	libmp3_dec.so libmp3_enc.so libmp3_lame.so \
	libot_mpi_isp.so libot_osal.so libsecurec.so \
	libss_ivs_md.so \
	libss_mpi.so libss_mpi_ae.so libss_mpi_audio.so libss_mpi_audio_adp.so \
	libss_mpi_awb.so libss_mpi_isp.so libss_mpi_ive.so \
	libss_mpi_sysbind.so libss_mpi_sysmem.so \
	libsvp_acl.so libupvqe.so libvoice_engine.so \
	libvqe_aec.so libvqe_agc.so libvqe_anr.so libvqe_eq.so \
	libvqe_hpf.so libvqe_hs.so libvqe_record.so libvqe_res.so libvqe_talkv2.so

# Sensor .so blobs shipped from vendor flash where the openhisilicon
# V5 SDK has no source mirror. Extracted from the original CV608 DEMO
# board flash (Hi3516CV610_MPP_V1.0.1.0 B040, Sep 2024). Source-built
# sensors (gc4023, os04d10, sc4336p, sc450ai, sc500ai, sc431hai, sp2308,
# imx335, imx662, cv2003, gc4653, vs4388)
# come from hisilicon-opensdk and are not duplicated here. The demo board's
# libsns_os02m10.so is gone: built for B040, it overran the B051 ISP's stack
# in pfn_cmos_get_standby_cfg and majestic aborted on every start. Its sensor
# (chip ID 0x5302) is the SP2308 source driver's.
HISILICON_OSDRV_HI3516CV6XX_SENSOR_BLOBS = \
	libsns_imx307.so

# hi3516cv6xx_lite ships the same 5120 KB squashfs slot on an 8 MB NOR part
# that master is already 0.68 KB short of (CI run 39116182919), so the IMX335
# and IMX662 profiles stay with hi3516cv6xx_ultimate (16 MB). The drivers are
# dropped from the sensor list in hisilicon-opensdk.mk; here only the profiles
# go away, which is why the gate is repeated: a profile without its driver is
# dead weight.
ifeq ($(OPENIPC_SOC_MODEL)/$(OPENIPC_VARIANT),hi3516cv6xx/lite)
HISILICON_OSDRV_HI3516CV6XX_SONY_PROFILES =
else
define HISILICON_OSDRV_HI3516CV6XX_SONY_PROFILES
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/sensor/config/imx335_*.ini
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/modes
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/modes $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/sensor/config/modes/*.ini
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/sensor/config/imx662_i2c_1080p.ini
endef
endif

define HISILICON_OSDRV_HI3516CV6XX_INSTALL_TARGET_CMDS

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/script/load*
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/script/set_allocator

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(foreach lib,$(HISILICON_OSDRV_HI3516CV6XX_MPP_LIBS), \
		$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/lib/$(lib) ; \
	)

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib/sensors
	$(foreach lib,$(HISILICON_OSDRV_HI3516CV6XX_SENSOR_BLOBS), \
		$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib/sensors $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/sensor/$(lib) ; \
	)

	# Sensor profiles. majestic, given no isp.sensorConfig, takes the one
	# file in /etc/sensors whose name starts with the detected sensor, so
	# imx335_i2c_5M.ini is the default. The alternate modes live in modes/
	# and are named without the imx335_i2c_ prefix so they cannot be picked
	# up by auto-selection. Their rates are the ones the T6 crop timing
	# actually reaches (1520p 30, flex 1920x1080 30, full10 40), not "high fps".
	# IMX662 has one, 1080p30 RAW12 on four lanes, RGGB, as the driver's
	# author runs it on a CV610. ipcinfo does not detect the IMX662, so a
	# camera needs `fw_setenv sensor imx662`; majestic then finds it by name.
	$(HISILICON_OSDRV_HI3516CV6XX_SONY_PROFILES)

	# SP2308, on both variants with its driver: ipcinfo names chip ID
	# 0x5302 sp2308, which is the name majestic looks this profile up by.
	# 1080p30 RAW10 on MIPI lanes 0 and 2, BGGR (checked under red, green
	# and blue light).
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/sensor/config/sp2308_i2c_1080p.ini

	# CV2003, GC4653 and VS4388, on both variants with their drivers: the
	# sensors of one vendor's CV610/CV608 cameras, which are 8 MB lite-class
	# boards (OpenIPC/majestic#355, #356, #357). Every one of them wires the
	# sensor to the second MIPI port in 2+2 mode on lanes 1 and 3, with its
	# clock on the second clock lane, into VI device 1 -- devno 1 and
	# lane_divide_mode 1 in the profile. RGGB, RAW10, linear: CV2003
	# 1080p30, GC4653 and VS4388 2560x1440p30. ipcinfo names them cv2003,
	# gc4653 and vs4388, the names majestic looks these profiles up by.
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors \
		$(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/sensor/config/cv2003_i2c_1080p.ini \
		$(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/sensor/config/gc4653_i2c_4M.ini \
		$(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/sensor/config/vs4388_i2c_4M.ini

endef

$(eval $(generic-package))
