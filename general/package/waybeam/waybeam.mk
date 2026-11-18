################################################################################
#
# waybeam
#
################################################################################

# Pinned, not HEAD: the mode tables and the sensor-plugin paths this arm relies
# on are properties of a commit, and an image built from a moving ref cannot be
# rebuilt or compared. 0.86.5 is the first release that carries the whole cv610
# stack (the `build` and `sensor-cv610` targets and the /usr/lib/cv610 IMX662
# entry in src/cv610_modes.c). Bump it with a rebuild, as for any other pin.
WAYBEAM_VERSION = f1085cbe9b4f0bf0fc50ababc5730ee1df044591
WAYBEAM_SITE = https://github.com/OpenIPC/waybeam.git
WAYBEAM_SITE_METHOD = git
# The SigmaStar sensor drivers include headers that are not in the waybeam
# tree itself — they come from the OpenIPC/sensors submodule at sensors-src/,
# so the checkout has to bring it along or drivers/ will not compile. The
# Hi3516CV610 backend compiles from this tree alone; the submodule comes along
# either way.
WAYBEAM_GIT_SUBMODULES = YES
WAYBEAM_LICENSE = MIT
WAYBEAM_LICENSE_FILES = LICENSE

# The encoder ships one source tree with a backend per SoC, picked by
# SOC_BUILD. Two are built here, one per SoC family:
#
#   star6e — SigmaStar Infinity6E (SSC338Q/SSC30KQ). Compiles its own
#     imx335/imx415 kernel drivers against the board kernel and installs them
#     over the stock _mipi.ko names the SigmaStar loader looks for.
#
#   cv610 — Hisilicon Hi3516CV610 (V5). Compiles against the public
#     OpenHisilicon V5 headers, links the vendor MPP userspace libraries and
#     drives the sensor as a userspace plugin it dlopens by name: from
#     /usr/lib/sensors, except IMX662, which it opens from /usr/lib/cv610 (see
#     the install step below). The kernel modules stay the image's own:
#     S70vendor has already loaded them through load_hisilicon by the time the
#     daemon starts, and the daemon re-initialises the graph around them
#     instead of touching the module set.
#
# The Infinity6C (maruko) backend is deliberately not built: it targets the
# 2024-06-18 SigmaStar MI libraries while sigmastar-osdrv-infinity6c ships
# 2022-09-07. Measured on an SSC378QE, the pipeline streams against those but
# AE never actuates and the frame comes out black (mean luma 2.05/255, against
# 153.91 on the 2024 set). Config.in therefore does not offer I6C, and it
# returns here once the two revisions agree.
ifeq ($(OPENIPC_SOC_FAMILY),hi3516cv6xx)
WAYBEAM_SOC = cv610

# The daemon is userspace: CV610_CC is the compiler, CV610_SDK_LIB the vendor
# MPP libraries it links against, and CV610_SDK_INC the OpenHisilicon source the
# headers are compiled from. CC_BIN is a star6e-only knob.
#
# The vendor MPP libraries no longer ship in an OpenIPC CV6xx image: majestic
# links the SDK from static archives, so OSDRV stopped installing the shared
# objects and nothing else in the image linked one. This backend does link
# them, so it takes the copies the OSDRV tree still carries and installs the
# ones it reaches below — for a waybeam image only.
WAYBEAM_CV610_MPP_LIB_DIR = $(HISILICON_OSDRV_HI3516CV6XX_PKGDIR)/files/lib

# The libraries to install, which is the set the daemon reaches rather than the
# 57 files OSDRV keeps: its cv610 link line (SOC_LIBS in the waybeam Makefile)
# closed over DT_NEEDED reaches 31 of them, 2.5 MB. The other 26 (NPU, IVE,
# crypto, OTP, UVC, the audio VQE set, mbedtls) are named by nothing on this
# backend. Listing them here keeps a future addition to OSDRV's files/lib from
# quietly joining every waybeam image, and keeps a name out of the list from
# failing only at runtime. libaac_comm, libmp3_lame, libot_osal and libsecurec
# are on it because the libraries above need them, not because the daemon names
# them; the VQE pair the audio path does use, libdnvqe and libupvqe, is here.
WAYBEAM_CV610_MPP_LIBS = \
	libaac_comm.so libaac_dec.so libaac_enc.so libaac_sbr_dec.so \
	libaac_sbr_enc.so libacs.so libbnr.so libcalcflicker.so libdehaze.so \
	libdnvqe.so libdrc.so libextend_stats.so libir_auto.so libldci.so \
	libmp3_dec.so libmp3_enc.so libmp3_lame.so libopus.so libot_mpi_isp.so \
	libot_osal.so libsecurec.so libss_mpi.so libss_mpi_ae.so \
	libss_mpi_audio.so libss_mpi_audio_adp.so libss_mpi_awb.so \
	libss_mpi_isp.so libss_mpi_sysbind.so libss_mpi_sysmem.so libupvqe.so \
	libvoice_engine.so

WAYBEAM_MAKE_OPTS = SOC_BUILD=cv610 \
	CV610_CC="$(TARGET_CC)" \
	CV610_SDK_LIB="$(WAYBEAM_CV610_MPP_LIB_DIR)"

# hisilicon-opensdk is a dependency for two things: it unpacks the
# OpenIPC/openhisilicon tree these sources compile against, and it installs the
# sensor plugins the daemon dlopens. No `linux` dependency: this arm builds no
# kernel module, so it does not care which kernel the board runs beyond the
# modules osdrv already loaded.
WAYBEAM_DEPENDENCIES += hisilicon-osdrv-hi3516cv6xx hisilicon-opensdk

# OpenHisilicon's V5 headers are not installed into the staging sysroot — the
# opensdk package builds a kernel module tree and the vendor libraries, not a
# userspace SDK — so the unpacked source tree has to be named. Buildroot already
# carries it as $(HISILICON_OPENSDK_DIR), which follows
# HISILICON_OPENSDK_OVERRIDE_SRCDIR; a directory of our own naming would go stale
# or pick up a second checkout. hisilicon-opensdk is in WAYBEAM_DEPENDENCIES, so
# it exists by the time this runs — the check below is for a tree that lost it.
#
# `build` rather than the tree's `stage` target, as on star6e: stage assembles a
# release tarball, and its extra work is unwanted in an image — it copies the
# vendor MPP .so set into that tarball (this package installs its own list below)
# and the sensor modules vendored into the source tree.
define WAYBEAM_BUILD_CMDS
	sdk=$(HISILICON_OPENSDK_DIR); \
	[ -d "$$sdk/kernel/include/hi3516cv6xx" ] || { \
		echo "ERROR: OpenHisilicon V5 headers not found at $$sdk" >&2; \
		exit 1; }; \
	$(MAKE) -C $(@D) build $(WAYBEAM_MAKE_OPTS) CV610_SDK_INC="$$sdk" && \
	$(MAKE) -C $(@D) sensor-cv610 $(WAYBEAM_MAKE_OPTS) CV610_SDK_INC="$$sdk"
endef

# The daemon dlopens /usr/lib/sensors/libsns_<type>.so by the sensor name it
# resolved, and hisilicon-opensdk installs those plugins -- except IMX662, which
# this backend takes from its own tree: src/cv610_modes.c names
# /usr/lib/cv610/libsns_imx662.so for it, because that plugin drives the sensor
# clock from the mode the backend picked and the openhisilicon copy assumes a
# fixed 37.125 MHz and refuses anything above 90 fps. hisilicon-opensdk filters
# its own IMX662 out when waybeam is selected, so exactly one build of that
# sensor ships.
# /etc/waybeam.json is waybeam's cv610 default: it encodes
# into the frame-shm ring venc_wfb, which wfb_tx drains. An image that streams
# over the network retargets outgoing.server (customiser, API); with nothing
# draining the ring the daemon's frame gate throttles the encoder instead of
# stalling it.
define WAYBEAM_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/out/$(WAYBEAM_SOC)/waybeam \
		$(TARGET_DIR)/usr/bin/waybeam
	$(INSTALL) -m 0644 -D $(@D)/config/waybeam.default.$(WAYBEAM_SOC).json \
		$(TARGET_DIR)/etc/waybeam.json
	# This backend's own IMX662 plugin, at the path src/cv610_modes.c names for
	# it. See the install comment above; hisilicon-opensdk drops its copy.
	$(INSTALL) -m 0755 -d $(TARGET_DIR)/usr/lib/cv610
	$(INSTALL) -m 0644 -D $(@D)/sensors/$(WAYBEAM_SOC)/imx662/libsns_imx662.so \
		$(TARGET_DIR)/usr/lib/cv610/libsns_imx662.so
	# The vendor MPP shared objects this backend links (see CV610_SDK_LIB):
	# an image needs them at runtime, and OSDRV no longer installs them.
	$(INSTALL) -m 0755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 0644 -t $(TARGET_DIR)/usr/lib \
		$(addprefix $(WAYBEAM_CV610_MPP_LIB_DIR)/,$(WAYBEAM_CV610_MPP_LIBS))
endef

else

WAYBEAM_SOC = star6e
# CC_BIN points the tree's own toolchain check at Buildroot's compiler so it
# does not fetch one of its own.
WAYBEAM_MAKE_OPTS = SOC_BUILD=star6e STAR6E_CC="$(TARGET_CC)" CC_BIN="$(TARGET_CC)"
# Built after the stock sensor and MI-library packages so the modified sensor
# modules install over the stock sensor_imx*_mipi.ko names.
WAYBEAM_DEPENDENCIES += sigmastar-osdrv-sensors sigmastar-osdrv-infinity6e

# The sensor modules are compiled here against the kernel this build just
# produced, so linux must be built first.
WAYBEAM_DEPENDENCIES += linux

# `build` rather than the tree's `stage` target: stage exists to assemble a
# release tarball, and its extra work is unwanted here. It copies the vendor
# MI .so files into the bundle, which OSDRV already installs to /usr/lib, and
# it stages sensor modules vendored into the source tree as prebuilt
# binaries. The modules below are compiled from drivers/*.c instead.
define WAYBEAM_BUILD_CMDS
	$(MAKE) -C $(@D) build json_cli $(WAYBEAM_MAKE_OPTS)
	$(MAKE) -C $(@D)/drivers sensor SOC=$(WAYBEAM_SOC) \
		KSRC="$(LINUX_DIR)" CROSS="$(TARGET_CROSS)"
endef

define WAYBEAM_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/out/$(WAYBEAM_SOC)/waybeam \
		$(TARGET_DIR)/usr/bin/waybeam
	$(INSTALL) -m 0755 -D $(@D)/out/$(WAYBEAM_SOC)/json_cli \
		$(TARGET_DIR)/usr/bin/json_cli
	$(INSTALL) -m 0644 -D $(WAYBEAM_PKGDIR)/files/waybeam.json \
		$(TARGET_DIR)/etc/waybeam.json
endef

# The encoder dlopens the SigmaStar MI libraries at runtime. They are
# installed to /usr/lib by sigmastar-osdrv-infinity6e, which does so only when
# Majestic is not selected; waybeam depends on !MAJESTIC, so that path always
# applies and the package ships no libraries itself. That the shipped MI
# revision is one the backend targets is a real constraint, not a formality --
# it is what rules out Infinity6C above.

# Waybeam drives its own imx335/imx415 drivers rather than the stock ones:
# they unlock higher-FPS sensor modes, up to 144 fps IMX335 / 100 fps IMX415.
# Install them over the stock _mipi.ko names the SigmaStar module loader looks
# for. LINUX_VERSION_PROBED rather than a literal 4.9.84 so the path follows
# whatever kernel the board actually built -- verified to resolve to the same
# directory the stock package writes to, which is what makes it an overwrite.
define WAYBEAM_INSTALL_SENSOR_MODULES
	$(INSTALL) -m 0644 -D $(@D)/drivers/sensor_imx335_$(WAYBEAM_SOC).ko \
		$(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/sigmastar/sensor_imx335_mipi.ko
	$(INSTALL) -m 0644 -D $(@D)/drivers/sensor_imx415_$(WAYBEAM_SOC).ko \
		$(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/sigmastar/sensor_imx415_mipi.ko
endef
WAYBEAM_POST_INSTALL_TARGET_HOOKS += WAYBEAM_INSTALL_SENSOR_MODULES

# No ISP tuning bins are installed. waybeam.json ships isp.sensorBin empty,
# and the encoder then resolves /etc/sensors/<sensor>.bin itself
# (src/pipeline_common.c) -- the same stock tuning Majestic loads, installed
# by sigmastar-osdrv-infinity6e and filtered on OPENIPC_SNS_MODEL. The bins
# vendored in the source tree under iq-profiles/ are not used: they were
# pulled off a running camera, which is the binary-without-source this tree
# refuses, and they are byte-identical to our own copies anyway.

endif

# The init script is backend-agnostic: it starts the daemon, which re-inits the
# graph on start and tears it down on stop, and never (re)loads modules. On
# CV610 that leaves the vendor module set S70vendor put up in place, which is
# what the backend runs on.
define WAYBEAM_INSTALL_INIT_SYSV
	$(INSTALL) -m 0755 -D $(WAYBEAM_PKGDIR)/files/S95waybeam \
		$(TARGET_DIR)/etc/init.d/S95waybeam
endef

$(eval $(generic-package))
