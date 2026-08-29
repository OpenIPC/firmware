################################################################################
#
# waybeam
#
################################################################################

WAYBEAM_VERSION = HEAD
WAYBEAM_SITE = https://github.com/OpenIPC/waybeam.git
WAYBEAM_SITE_METHOD = git
# The sensor drivers include SigmaStar headers that are not in the waybeam
# tree itself — they come from the OpenIPC/sensors submodule at sensors-src/,
# so the checkout has to bring it along or drivers/ will not compile.
WAYBEAM_GIT_SUBMODULES = YES
WAYBEAM_LICENSE = MIT
WAYBEAM_LICENSE_FILES = LICENSE

# The encoder ships one source tree with a backend per SoC, picked by
# SOC_BUILD. Only the Infinity6E (star6e) backend is built here. The tree also
# has a maruko backend for Infinity6C, but it targets the 2024-06-18 SigmaStar
# MI libraries while sigmastar-osdrv-infinity6c ships 2022-09-07: measured on
# an SSC378QE, the pipeline streams against those but AE never actuates and
# the frame comes out black (mean luma 2.05/255, against 153.91 on the 2024
# set). Config.in therefore does not offer I6C, and it returns here once the
# two revisions agree.
#
# CC_BIN points the tree's own toolchain check at Buildroot's compiler so it
# does not fetch one of its own.
WAYBEAM_SOC = star6e
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

define WAYBEAM_INSTALL_INIT_SYSV
	$(INSTALL) -m 0755 -D $(WAYBEAM_PKGDIR)/files/S95waybeam \
		$(TARGET_DIR)/etc/init.d/S95waybeam
endef

$(eval $(generic-package))
