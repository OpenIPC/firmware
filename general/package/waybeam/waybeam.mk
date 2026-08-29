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

# The encoder ships a single source tree with two SoC backends selected by
# SOC_BUILD. Derive it from the target SoC family and build with Buildroot's
# toolchain; CC_BIN / CC_MARUKO_BIN point the tree's toolchain check at the
# same compiler so it does not fetch its own.
ifeq ($(OPENIPC_SOC_FAMILY),infinity6c)
WAYBEAM_SOC = maruko
WAYBEAM_MAKE_OPTS = SOC_BUILD=maruko MARUKO_CC="$(TARGET_CC)" CC_MARUKO_BIN="$(TARGET_CC)"
# Built after the stock sensor and MI-library packages so the modified sensor
# modules install over the stock sensor_imx*_mipi.ko names.
WAYBEAM_DEPENDENCIES += sigmastar-osdrv-sensors sigmastar-osdrv-infinity6c
else
WAYBEAM_SOC = star6e
WAYBEAM_MAKE_OPTS = SOC_BUILD=star6e STAR6E_CC="$(TARGET_CC)" CC_BIN="$(TARGET_CC)"
WAYBEAM_DEPENDENCIES += sigmastar-osdrv-sensors sigmastar-osdrv-infinity6e
endif

# The sensor modules are compiled here against the kernel this build just
# produced, so linux must be built first.
WAYBEAM_DEPENDENCIES += linux

# `build` rather than the tree's `stage` target: stage exists to assemble a
# release tarball, and its extra work is either unwanted or unusable here. It
# copies the vendor MI .so files into the bundle (OSDRV already installs those
# to /usr/lib, and this package deliberately ships no libraries), and on
# Infinity6C it stages sensor modules and ISP bins that are vendored into the
# source tree as prebuilt binaries. Both sensor modules below are compiled
# from drivers/*.c instead.
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
# installed to /usr/lib by sigmastar-osdrv-infinity6{e,c}, which does so
# only when Majestic is not selected; waybeam depends on !MAJESTIC, so
# that path always applies and the package ships no libraries itself.

# Waybeam drives its own imx335/imx415 drivers rather than the stock ones.
# On Infinity6E they unlock higher-FPS sensor modes (up to 144 fps IMX335 /
# 100 fps IMX415). On Infinity6C stock drivers do exist -- OpenIPC/sensors
# carries sensor_imx{335,415}_mipi.c -- and these add fixes those lack: a
# no-op poweroff, because an MI_SNR power cycle breaks IMX335 MIPI sync on
# I6C, a boot-mode pre-transition, and an AE VTS cap. Install them over the
# stock _mipi.ko names the SigmaStar module loader looks for.
# LINUX_VERSION_PROBED rather than a literal 4.9.84 / 5.10.61 so the path
# follows whatever kernel the board actually built.
define WAYBEAM_INSTALL_SENSOR_MODULES
	$(INSTALL) -m 0644 -D $(@D)/drivers/sensor_imx335_$(WAYBEAM_SOC).ko \
		$(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/sigmastar/sensor_imx335_mipi.ko
	$(INSTALL) -m 0644 -D $(@D)/drivers/sensor_imx415_$(WAYBEAM_SOC).ko \
		$(TARGET_DIR)/lib/modules/$(LINUX_VERSION_PROBED)/sigmastar/sensor_imx415_mipi.ko
endef
WAYBEAM_POST_INSTALL_TARGET_HOOKS += WAYBEAM_INSTALL_SENSOR_MODULES

# No ISP tuning bins are installed. The ones under iq-profiles/maruko-bin/ in
# the source tree were pulled off a running SSC378QE, which is the
# binary-without-source this tree refuses -- and they are byte-identical to
# our own copies in sigmastar-osdrv-infinity6c, so vendoring them only
# round-tripped this repository's files back at it. Image quality does not
# regress by leaving them out: with isp.sensorBin empty waybeam resolves
# /etc/sensors/<sensor>.bin itself (src/pipeline_common.c), which is the same
# stock tuning Majestic loads. sigmastar-osdrv-infinity6c installs it,
# filtered on OPENIPC_SNS_MODEL, so a board that pins a sensor ships only that
# sensor's bin -- right for a shipped board, though a sensor-swap bench has to
# copy the other one across by hand.

define WAYBEAM_INSTALL_INIT_SYSV
	$(INSTALL) -m 0755 -D $(WAYBEAM_PKGDIR)/files/S95waybeam \
		$(TARGET_DIR)/etc/init.d/S95waybeam
endef

$(eval $(generic-package))
