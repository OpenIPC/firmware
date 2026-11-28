################################################################################
#
# goke-osdrv-gk7206v100
#
################################################################################

GOKE_OSDRV_GK7206V100_VERSION =
GOKE_OSDRV_GK7206V100_SITE =
# The vendor's binaries, redistributed as the SDK publishes them; the SDK
# repository states no license for them, so none is claimed here.
GOKE_OSDRV_GK7206V100_LICENSE = PROPRIETARY
# hisilicon-opensdk links its dump tools against these.
GOKE_OSDRV_GK7206V100_INSTALL_STAGING = YES

# The vendor userspace is linked against uClibc-ng: every library NEEDs
# libc.so.0 and most ld-uClibc.so.1, and it speaks lower-case xmedia_*
# (there is no HiSilicon-named shim, so majestic cannot use it). It runs on
# musl the way the hi3516cv100 SDK does, through uclibc-compat. It is also
# hard-float, which is why gk7206v100 is a musleabihf board.
GOKE_OSDRV_GK7206V100_DEPENDENCIES = uclibc-compat

# Provenance: ShiMetaPi GK7206 SDK V1.0.0 (github.com/ShiMetaPi/
# shimetapi_pico_gx, 8d691107f3ea), source/gmp/lib/
# arm-gcc12.2.0-linux-uclibceabi/nolog/share/, copied unmodified.
# files/lib/SHA256SUMS pins them and the install checks it, so a swapped or
# truncated blob fails the build instead of shipping.
#
# Only what the SDK has no source for is here. hisilicon-opensdk builds the
# rest from the SDK's own source: the kernel modules it can (relinking the
# others), the libsns_* sensor drivers, libxmedia_msensor, libxmedia_svp and
# the VI/VPSS/VO dump tools (built, not installed: capture tools belong in
# ipctool).
#
# The libraries declare no NEEDED on one another -- the SDK leaves it to the
# program to link every one -- and musl binds every symbol at load time, so a
# set has to be closed over the symbols they import, not over NEEDED. That is
# why the lite set carries the NPU graph runtime (cl, npu, flatcc): the ISP
# calls it for AI noise reduction. Closure checked against musl +
# uclibc-compat on 2026-10-14.
GOKE_OSDRV_GK7206V100_LIBS = liblens libxmedia_ae libxmedia_af libxmedia_awb \
	libxmedia_cipher libxmedia_cl libxmedia_common libxmedia_flatcc \
	libxmedia_isp libxmedia_ive libxmedia_md libxmedia_npu libxmedia_pm \
	libxmedia_region libxmedia_tde libxmedia_venc libxmedia_vgs libxmedia_vi \
	libxmedia_vo libxmedia_vpss libxmedia_audio libxmedia_audio_g711_codec \
	libxmedia_audio_vqe libxmedia_audio_vqe_sv_common \
	libxmedia_audio_vqe_sv_gain

# The AAC, MP3, G.726, ADPCM and the other VQE algorithm libraries add 2.3MB
# that nothing in a lite image calls; ultimate gets them for user programs.
ifeq ($(OPENIPC_VARIANT),ultimate)
GOKE_OSDRV_GK7206V100_LIBS = $(basename $(notdir $(wildcard $(GOKE_OSDRV_GK7206V100_PKGDIR)/files/lib/*.so)))
endif

define GOKE_OSDRV_GK7206V100_INSTALL_STAGING_CMDS
	cd $(GOKE_OSDRV_GK7206V100_PKGDIR)/files/lib && sha256sum -c --quiet SHA256SUMS
	$(INSTALL) -m 755 -d $(STAGING_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(STAGING_DIR)/usr/lib $(GOKE_OSDRV_GK7206V100_PKGDIR)/files/lib/*.so
endef

define GOKE_OSDRV_GK7206V100_INSTALL_TARGET_CMDS
	cd $(GOKE_OSDRV_GK7206V100_PKGDIR)/files/lib && sha256sum -c --quiet SHA256SUMS

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(foreach l,$(GOKE_OSDRV_GK7206V100_LIBS), \
		$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(GOKE_OSDRV_GK7206V100_PKGDIR)/files/lib/$(l).so ; \
	)

	# The names this SDK's uClibc-ng libraries NEED. rootfs_script.sh only
	# links ld-uClibc.so.0, for the older SDKs. Both resolve to musl.
	ln -sf libc.so $(TARGET_DIR)/lib/ld-uClibc.so.1
	ln -sf libc.so $(TARGET_DIR)/lib/libc.so.0

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(GOKE_OSDRV_GK7206V100_PKGDIR)/files/script/load_goke

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(GOKE_OSDRV_GK7206V100_PKGDIR)/files/sensor/config/*.ini

	# Image tuning, as text profiles: [section] key = values over the
	# SDK's ISP, AE and AWB attribute structures, the keys their member
	# paths. The streamer reads /etc/sensors/iq/<sensor>.ini (isp.iqProfile
	# overrides the path); a key not written keeps the SDK's value.
	#
	# sc465sl.ini carries the tuning the vendor's own firmware applies to an
	# SC465SL on GK7206V11 (Xiongmai IPC_GK7206V11_K8, V1.10.T02.20261103):
	# its per-ISO noise reduction, sharpening, demosaic, defect-pixel,
	# gamma, DRC, dehaze, LCE and colour tables, its AWB calibration and its
	# AE compensation curve. The SDK publishes no tuning for this sensor;
	# without it every block runs on the ISP library's generic defaults.
	# The values are vendor-derived, not from a datasheet: read from that
	# firmware running in a QEMU twin (OpenIPC/qemu-bench f0c2fcbe5,
	# tools/xm-isp-tuning) as a PQ bin, then exported as text by the
	# streamer (GET /api/v1/isp/profile.ini); the file's header has the
	# steps. The text is the source from here on. Checked against the PQ bin
	# it replaces on an IVG-K8: the same attribute blocks and ISP state, but
	# for white balance and exposure. The [awb*] and [ae*] sections are read
	# live from a stock camera in daylight, because the dump caught the App
	# before it set them (colour matrix in manual, washed-out colour, a cyan
	# cast); the AE compensation table is fitted to the stock camera's
	# target at four gains, since its App rewrites that table as gain moves.
	# DRC is the App's own per-ISO table for this sensor (read from its
	# binary, checked against the stock camera's live strength), run as the
	# SDK's DRC auto mode; AINR runs in auto, as on the stock camera.
	#
	# sc465sl.night.ini is the vendor's IR night set as a delta over the day
	# profile (night sharpening, noise reduction, LCE, dehaze, DRC strength,
	# AWB bypassed), read from the App's own ISP code with its day/night
	# switch called each way. A streamer without night profiles ignores it.
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors/iq
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors/iq $(GOKE_OSDRV_GK7206V100_PKGDIR)/files/iq/*.ini
endef

$(eval $(generic-package))
