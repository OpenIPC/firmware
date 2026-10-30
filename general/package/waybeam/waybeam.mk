################################################################################
#
# waybeam
#
################################################################################

WAYBEAM_VERSION = HEAD
WAYBEAM_SITE = https://github.com/OpenIPC/waybeam.git
WAYBEAM_SITE_METHOD = git
# The SigmaStar sensor drivers include headers that are not in the waybeam
# tree itself — they come from the OpenIPC/sensors submodule at sensors-src/,
# so the checkout has to bring it along or drivers/ will not compile. The
# Hi3516CV610 backend compiles from the tree alone, but the submodule comes
# along either way.
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
#     drives its sensors as userspace plugins (dlopened by name), not kernel
#     modules.
#
# The Infinity6C (maruko) backend is deliberately not built: it targets the
# 2024-06-18 SigmaStar MI libraries while sigmastar-osdrv-infinity6c ships
# 2022-09-07. Measured on an SSC378QE, the pipeline streams against those but
# AE never actuates and the frame comes out black (mean luma 2.05/255, against
# 153.91 on the 2024 set). Config.in therefore does not offer I6C, and it
# returns here once the two revisions agree.
ifeq ($(OPENIPC_SOC_FAMILY),hi3516cv6xx)
WAYBEAM_SOC = cv610
WAYBEAM_CONFIG = files/waybeam.cv610.json
# CV610_CC points the tree's own toolchain fetch at Buildroot's compiler, so
# it does not fetch one of its own. CV610_SDK_INC is the OpenHisilicon source
# the opensdk package unpacks; CV610_SDK_LIB is where
# hisilicon-osdrv-hi3516cv6xx installs the vendor MPP .so files. CC_BIN is a
# star6e-only knob.
#
# $(TARGET_DIR)/usr/lib is the right path under per-package directories too:
# before this package configures, Buildroot rsyncs every dependency's target
# tree into this package's $(TARGET_DIR) (prepare-per-package-directory in
# package/pkg-utils.mk), so hisilicon-osdrv-hi3516cv6xx's libss_mpi*.so are
# already there. Reading them out of that package's own per-package tree
# instead would bypass that mechanism.
WAYBEAM_MAKE_OPTS = SOC_BUILD=cv610 \
	CV610_CC="$(TARGET_CC)" \
	CV610_SDK_INC="$(HISILICON_OPENSDK_DIR)" \
	CV610_SDK_LIB="$(TARGET_DIR)/usr/lib"
# Built after the stock sensor and MPP-library packages: this tree's IMX662
# plugin takes the /usr/lib/sensors/libsns_imx662.so path that
# hisilicon-opensdk would fill from OpenIPC/openhisilicon, so that copy is
# filtered out at selection time when waybeam is on (hisilicon-opensdk.mk,
# next to the lite filter). The vendor .so set the backend links against is
# installed by hisilicon-osdrv-hi3516cv6xx.
WAYBEAM_DEPENDENCIES += hisilicon-opensdk hisilicon-osdrv-hi3516cv6xx
else
WAYBEAM_SOC = star6e
WAYBEAM_CONFIG = files/waybeam.json
# CC_BIN points the tree's own toolchain check at Buildroot's compiler so it
# does not fetch one of its own.
WAYBEAM_MAKE_OPTS = SOC_BUILD=star6e STAR6E_CC="$(TARGET_CC)" CC_BIN="$(TARGET_CC)"
# Built after the stock sensor and MI-library packages so the modified sensor
# modules install over the stock sensor_imx*_mipi.ko names.
WAYBEAM_DEPENDENCIES += sigmastar-osdrv-sensors sigmastar-osdrv-infinity6e
# The sensor modules are compiled here against the kernel this build just
# produced, so linux must be built first. The CV610 branch needs no kernel of
# its own: it builds userspace plugins against the OpenHisilicon headers.
WAYBEAM_DEPENDENCIES += linux
endif

# `build` rather than the tree's `stage` target: stage exists to assemble a
# release tarball, and its extra work is unwanted here. It copies the vendor
# MI .so files into the bundle, which OSDRV already installs to /usr/lib, and
# it stages sensor modules, the CV610 loader and the release config as
# prebuilt files. What this package ships is compiled from source instead.
ifeq ($(OPENIPC_SOC_FAMILY),hi3516cv6xx)
define WAYBEAM_BUILD_CMDS
	$(MAKE) -C $(@D) build $(WAYBEAM_MAKE_OPTS)
	$(MAKE) -C $(@D) sensor-cv610 $(WAYBEAM_MAKE_OPTS)
endef
else
define WAYBEAM_BUILD_CMDS
	$(MAKE) -C $(@D) build json_cli $(WAYBEAM_MAKE_OPTS)
	$(MAKE) -C $(@D)/drivers sensor SOC=$(WAYBEAM_SOC) \
		KSRC="$(LINUX_DIR)" CROSS="$(TARGET_CROSS)"
endef
endif

ifeq ($(OPENIPC_SOC_FAMILY),hi3516cv6xx)
define WAYBEAM_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/out/cv610/waybeam \
		$(TARGET_DIR)/usr/bin/waybeam
	# The backend dlopens /usr/lib/sensors/libsns_<type>.so by the configured
	# sensor name. IMX662 ships from this tree, because this is the copy the
	# backend's mode table and ISP seeds were developed against;
	# hisilicon-opensdk is told to skip its own build of the same sensor, so
	# exactly one package owns the path instead of the winner being whatever
	# target-finalize's sorted rsync lays down last. IMX335 has no copy here
	# at all: hisilicon-opensdk builds it from OpenIPC/openhisilicon
	# (libraries/sensor/hi3516cv6xx/sony_imx335), and the daemon's side of
	# that contract is the mode table in src/cv610_modes.c.
	$(INSTALL) -m 0755 -d $(TARGET_DIR)/usr/lib/sensors
	$(INSTALL) -m 0644 -D $(@D)/sensors/cv610/imx662/libsns_imx662.so \
		$(TARGET_DIR)/usr/lib/sensors/libsns_imx662.so
	$(INSTALL) -m 0644 -D $(WAYBEAM_PKGDIR)/$(WAYBEAM_CONFIG) \
		$(TARGET_DIR)/etc/waybeam.json
endef
else
define WAYBEAM_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/out/$(WAYBEAM_SOC)/waybeam \
		$(TARGET_DIR)/usr/bin/waybeam
	$(INSTALL) -m 0755 -D $(@D)/out/$(WAYBEAM_SOC)/json_cli \
		$(TARGET_DIR)/usr/bin/json_cli
	$(INSTALL) -m 0644 -D $(WAYBEAM_PKGDIR)/$(WAYBEAM_CONFIG) \
		$(TARGET_DIR)/etc/waybeam.json
endef

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

# The encoder dlopens the SigmaStar MI libraries at runtime. They are
# installed to /usr/lib by sigmastar-osdrv-infinity6e, which does so only when
# Majestic is not selected; waybeam depends on !MAJESTIC, so that path always
# applies and the package ships no libraries itself. That the shipped MI
# revision is one the backend targets is a real constraint, not a formality --
# it is what rules out Infinity6C above.
endif

# On Hi3516CV610 the sensors are userspace plugins built from the waybeam
# tree (above), so the SigmaStar-only blocks -- the unlocked kernel modules and
# the /etc/sensors tuning lookup -- do not apply: the CV610 backend carries its
# own per-mode tables and reads no .ini/.bin from /etc/sensors. No ISP tuning
# bins are installed on either backend: the SigmaStar backend resolves
# /etc/sensors/<sensor>.bin itself (the stock tuning Majestic loads, installed
# by sigmastar-osdrv-infinity6e and filtered on OPENIPC_SNS_MODEL), and the
# bins vendored in the tree under iq-profiles/ are the binary-without-source
# this tree refuses.

# The init script is backend-agnostic: it starts the daemon, which re-inits the
# graph on start and tears it down on stop, and never (re)loads modules. On
# CV610 that means the vendor modules S70vendor put up stay up, which is what
# the backend runs on.
define WAYBEAM_INSTALL_INIT_SYSV
	$(INSTALL) -m 0755 -D $(WAYBEAM_PKGDIR)/files/S95waybeam \
		$(TARGET_DIR)/etc/init.d/S95waybeam
endef

$(eval $(generic-package))
