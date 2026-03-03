################################################################################
#
# apfpv-greg-msposd
#
################################################################################

APFPV_GREG_MSPOSD_SITE = $(patsubst %/,%,$(APFPV_GREG_MSPOSD_PKGDIR))
APFPV_GREG_MSPOSD_SITE_METHOD = local
APFPV_GREG_MSPOSD_LICENSE = Mixed
APFPV_GREG_MSPOSD_GIT = $(shell command -v git 2>/dev/null)
APFPV_GREG_MSPOSD_REPO_BRANCH = main
APFPV_GREG_MSPOSD_REPO = https://github.com/sickgreg/msposd.git

ifeq ($(OPENIPC_SOC_FAMILY),infinity6b0)
	APFPV_GREG_MSPOSD_FAMILY = star6b0
	APFPV_GREG_MSPOSD_OSDRV = $(SIGMASTAR_OSDRV_INFINITY6B0_PKGDIR)
	APFPV_GREG_MSPOSD_OSDRV_PKG = sigmastar-osdrv-infinity6b0
else ifeq ($(OPENIPC_SOC_FAMILY),infinity6c)
	APFPV_GREG_MSPOSD_FAMILY = star6c
	APFPV_GREG_MSPOSD_OSDRV = $(SIGMASTAR_OSDRV_INFINITY6C_PKGDIR)
	APFPV_GREG_MSPOSD_OSDRV_PKG = sigmastar-osdrv-infinity6c
else
	APFPV_GREG_MSPOSD_FAMILY = star6e
	APFPV_GREG_MSPOSD_OSDRV = $(SIGMASTAR_OSDRV_INFINITY6E_PKGDIR)
	APFPV_GREG_MSPOSD_OSDRV_PKG = sigmastar-osdrv-infinity6e
endif

APFPV_GREG_MSPOSD_DEPENDENCIES = libevent-openipc $(APFPV_GREG_MSPOSD_OSDRV_PKG)

define APFPV_GREG_MSPOSD_FETCH_REPO
	test -n "$(APFPV_GREG_MSPOSD_GIT)" || { \
		echo "git is required to build apfpv-greg-msposd"; \
		exit 1; \
	}
	rm -rf $(@D)/src
	$(APFPV_GREG_MSPOSD_GIT) clone --depth 1 --branch $(APFPV_GREG_MSPOSD_REPO_BRANCH) $(APFPV_GREG_MSPOSD_REPO) $(@D)/src
endef

APFPV_GREG_MSPOSD_PRE_CONFIGURE_HOOKS += APFPV_GREG_MSPOSD_FETCH_REPO

define APFPV_GREG_MSPOSD_BUILD_CMDS
	$(MAKE) \
		CC="$(TARGET_CC)" \
		TOOLCHAIN="$(STAGING_DIR)" \
		DRV="$(APFPV_GREG_MSPOSD_OSDRV)/files/lib" \
		$(APFPV_GREG_MSPOSD_FAMILY) \
		OUTPUT="$(@D)/msposd" \
		-C $(@D)/src
endef

define APFPV_GREG_MSPOSD_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(@D)/src/vtxmenu.ini
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(APFPV_GREG_MSPOSD_PKGDIR)/files/S99msposd
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/msposd
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/fonts
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/src/fonts/font_inav.png
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/src/fonts/font_inav_hd.png
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/src/fonts/font_btfl.png
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/src/fonts/font_btfl_hd.png
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/src/fonts/font_ardu.png
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/src/fonts/font_ardu_hd.png
endef

define APFPV_GREG_MSPOSD_INSTALL_SIGMASTAR_LIBRARIES
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(APFPV_GREG_MSPOSD_OSDRV)/files/lib/libcam_os_wrapper.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(APFPV_GREG_MSPOSD_OSDRV)/files/lib/libmi_rgn.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(APFPV_GREG_MSPOSD_OSDRV)/files/lib/libmi_sys.so
endef

ifeq ($(OPENIPC_SOC_VENDOR),sigmastar)
APFPV_GREG_MSPOSD_POST_INSTALL_TARGET_HOOKS += APFPV_GREG_MSPOSD_INSTALL_SIGMASTAR_LIBRARIES
endif

$(eval $(generic-package))
