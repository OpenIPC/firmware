################################################################################
#
# apfpv-greg10
#
################################################################################

APFPV_GREG10_SITE = $(patsubst %/,%,$(APFPV_GREG10_PKGDIR))
APFPV_GREG10_SITE_METHOD = local
APFPV_GREG10_LICENSE = Mixed
APFPV_GREG10_GIT = $(shell command -v git 2>/dev/null)
APFPV_GREG10_REPO_BRANCH = main
APFPV_GREG10_AALINK_REPO = https://github.com/sickgreg/aalink.git
APFPV_GREG10_AALINK_IDR5_REPO = https://github.com/sickgreg/aalink_idr5.git
APFPV_GREG10_AIR_MAN_REPO = https://github.com/sickgreg/air_man_ap.git
APFPV_GREG10_MSPOSD_REPO = https://github.com/sickgreg/msposd.git
APFPV_GREG10_PAYLOAD_REPO = https://github.com/sickgreg/apfpv-greg10-package.git

ifeq ($(OPENIPC_SOC_FAMILY),infinity6b0)
	APFPV_GREG10_MSPOSD_FAMILY = star6b0
	APFPV_GREG10_MSPOSD_OSDRV = $(SIGMASTAR_OSDRV_INFINITY6B0_PKGDIR)
else ifeq ($(OPENIPC_SOC_FAMILY),infinity6c)
	APFPV_GREG10_MSPOSD_FAMILY = star6c
	APFPV_GREG10_MSPOSD_OSDRV = $(SIGMASTAR_OSDRV_INFINITY6C_PKGDIR)
else
	APFPV_GREG10_MSPOSD_FAMILY = star6e
	APFPV_GREG10_MSPOSD_OSDRV = $(SIGMASTAR_OSDRV_INFINITY6E_PKGDIR)
endif

ifeq ($(OPENIPC_SOC_VENDOR),sigmastar)
APFPV_GREG10_PLATFORM_PAYLOAD = sigmastar-4.9.84
endif

define APFPV_GREG10_FETCH_REPOS
	test -n "$(APFPV_GREG10_GIT)" || { \
		echo "git is required to build apfpv-greg10"; \
		exit 1; \
	}
	rm -rf $(@D)/aalink-src $(@D)/aalink-idr5-src $(@D)/air-man-src $(@D)/msposd-src $(@D)/payload-src
	$(APFPV_GREG10_GIT) clone --depth 1 --branch $(APFPV_GREG10_REPO_BRANCH) $(APFPV_GREG10_AALINK_REPO) $(@D)/aalink-src
	$(APFPV_GREG10_GIT) clone --depth 1 --branch $(APFPV_GREG10_REPO_BRANCH) $(APFPV_GREG10_AALINK_IDR5_REPO) $(@D)/aalink-idr5-src
	$(APFPV_GREG10_GIT) clone --depth 1 --branch $(APFPV_GREG10_REPO_BRANCH) $(APFPV_GREG10_AIR_MAN_REPO) $(@D)/air-man-src
	$(APFPV_GREG10_GIT) clone --depth 1 --branch $(APFPV_GREG10_REPO_BRANCH) $(APFPV_GREG10_MSPOSD_REPO) $(@D)/msposd-src
	$(APFPV_GREG10_GIT) clone --depth 1 --branch $(APFPV_GREG10_REPO_BRANCH) $(APFPV_GREG10_PAYLOAD_REPO) $(@D)/payload-src
endef

APFPV_GREG10_PRE_CONFIGURE_HOOKS += APFPV_GREG10_FETCH_REPOS

define APFPV_GREG10_BUILD_CMDS
	$(TARGET_CC) $(TARGET_CFLAGS) -O2 -Wall -Wextra \
		-o $(@D)/aalink \
		$(@D)/aalink-src/aalink.c \
		$(TARGET_LDFLAGS) -lpthread -lm
	$(TARGET_CC) $(TARGET_CFLAGS) -O2 -Wall -Wextra \
		-o $(@D)/aalink_idr5 \
		$(@D)/aalink-idr5-src/aalink_idr5.c \
		$(TARGET_LDFLAGS)
	$(TARGET_CC) $(TARGET_CFLAGS) -O2 -Wall -Wextra -pthread \
		-o $(@D)/air_man_ap \
		$(@D)/air-man-src/air_man_ap.c \
		$(TARGET_LDFLAGS)
	$(MAKE) \
		CC="$(TARGET_CC)" \
		TOOLCHAIN="$(STAGING_DIR)" \
		DRV="$(APFPV_GREG10_MSPOSD_OSDRV)/files/lib" \
		$(APFPV_GREG10_MSPOSD_FAMILY) \
		OUTPUT="$(@D)/msposd" \
		-C $(@D)/msposd-src
endef

define APFPV_GREG10_INSTALL_TARGET_CMDS
	cp -a $(@D)/payload-src/payload/common/. $(TARGET_DIR)/
	rm -f \
		$(TARGET_DIR)/etc/init.d/S40network \
		$(TARGET_DIR)/etc/init.d/S95majestic \
		$(TARGET_DIR)/etc/init.d/S99rc.local \
		$(TARGET_DIR)/etc/network/interfaces.d/wlan0 \
		$(TARGET_DIR)/usr/share/udhcpc/default.script \
		$(TARGET_DIR)/var/www/cgi-bin/p/header.cgi
	chmod 755 \
		$(TARGET_DIR)/etc/init.d/S993video_settings \
		$(TARGET_DIR)/etc/init.d/media_stack \
		$(TARGET_DIR)/usr/bin/adapter \
		$(TARGET_DIR)/usr/share/openipc/customizer.sh

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(@D)/aalink-src/aalink.conf
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(@D)/msposd-src/vtxmenu.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(APFPV_GREG10_PKGDIR)/files/libmedia.sh
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(APFPV_GREG10_PKGDIR)/files/S94idr
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(APFPV_GREG10_PKGDIR)/files/S991aalink
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(APFPV_GREG10_PKGDIR)/files/S992air_man
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(APFPV_GREG10_PKGDIR)/files/S99msposd

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(@D)/air-man-src/modes_imx335.ini
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(@D)/air-man-src/modes_imx415.ini

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/aalink
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/aalink_idr5
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/air_man_ap
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/msposd

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/fonts
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/msposd-src/fonts/font_inav.png
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/msposd-src/fonts/font_inav_hd.png
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/msposd-src/fonts/font_btfl.png
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/msposd-src/fonts/font_btfl_hd.png
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/msposd-src/fonts/font_ardu.png
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/share/fonts $(@D)/msposd-src/fonts/font_ardu_hd.png
	if [ -n "$(APFPV_GREG10_PLATFORM_PAYLOAD)" ] && [ -d $(@D)/payload-src/payload/$(APFPV_GREG10_PLATFORM_PAYLOAD) ]; then \
		cp -a $(@D)/payload-src/payload/$(APFPV_GREG10_PLATFORM_PAYLOAD)/. $(TARGET_DIR)/; \
	fi
	rm -f $(TARGET_DIR)/lib/modules/4.9.84/sigmastar/sensor_imx415_mipi.ko
endef

define APFPV_GREG10_INSTALL_SIGMASTAR_LIBRARIES
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(APFPV_GREG10_MSPOSD_OSDRV)/files/lib/libcam_os_wrapper.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(APFPV_GREG10_MSPOSD_OSDRV)/files/lib/libmi_rgn.so
	$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib $(APFPV_GREG10_MSPOSD_OSDRV)/files/lib/libmi_sys.so
endef

ifeq ($(OPENIPC_SOC_VENDOR),sigmastar)
APFPV_GREG10_POST_INSTALL_TARGET_HOOKS += APFPV_GREG10_INSTALL_SIGMASTAR_LIBRARIES
endif

$(eval $(generic-package))
