################################################################################
#
# apfpv-greg-payload
#
################################################################################

APFPV_GREG_PAYLOAD_SITE = $(patsubst %/,%,$(APFPV_GREG_PAYLOAD_PKGDIR))
APFPV_GREG_PAYLOAD_SITE_METHOD = local
APFPV_GREG_PAYLOAD_LICENSE = Mixed
APFPV_GREG_PAYLOAD_GIT = $(shell command -v git 2>/dev/null)
APFPV_GREG_PAYLOAD_REPO_BRANCH = main
APFPV_GREG_PAYLOAD_REPO = https://github.com/sickgreg/apfpv-greg10-package.git

ifeq ($(OPENIPC_SOC_VENDOR),sigmastar)
APFPV_GREG_PAYLOAD_PLATFORM_PAYLOAD = sigmastar-4.9.84
endif

define APFPV_GREG_PAYLOAD_FETCH_REPO
	test -n "$(APFPV_GREG_PAYLOAD_GIT)" || { \
		echo "git is required to build apfpv-greg-payload"; \
		exit 1; \
	}
	rm -rf $(@D)/payload-src
	$(APFPV_GREG_PAYLOAD_GIT) clone --depth 1 --branch $(APFPV_GREG_PAYLOAD_REPO_BRANCH) $(APFPV_GREG_PAYLOAD_REPO) $(@D)/payload-src
endef

APFPV_GREG_PAYLOAD_PRE_CONFIGURE_HOOKS += APFPV_GREG_PAYLOAD_FETCH_REPO

define APFPV_GREG_PAYLOAD_INSTALL_TARGET_CMDS
	cp -a $(@D)/payload-src/payload/common/. $(TARGET_DIR)/
	rm -f \
		$(TARGET_DIR)/etc/init.d/S40network \
		$(TARGET_DIR)/etc/init.d/S95majestic \
		$(TARGET_DIR)/etc/init.d/S99rc.local \
		$(TARGET_DIR)/etc/network/interfaces.d/wlan0 \
		$(TARGET_DIR)/usr/share/udhcpc/default.script \
		$(TARGET_DIR)/var/www/cgi-bin/p/header.cgi
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(APFPV_GREG_PAYLOAD_PKGDIR)/files/libmedia.sh
	chmod 755 \
		$(TARGET_DIR)/etc/init.d/S993video_settings \
		$(TARGET_DIR)/etc/init.d/media_stack \
		$(TARGET_DIR)/usr/bin/adapter \
		$(TARGET_DIR)/usr/share/openipc/customizer.sh
	if [ -d $(TARGET_DIR)/var/www/cgi-bin ]; then \
		find $(TARGET_DIR)/var/www/cgi-bin -type f -name '*.cgi' -exec chmod 755 {} +; \
	fi
	if [ -n "$(APFPV_GREG_PAYLOAD_PLATFORM_PAYLOAD)" ] && [ -d $(@D)/payload-src/payload/$(APFPV_GREG_PAYLOAD_PLATFORM_PAYLOAD) ]; then \
		cp -a $(@D)/payload-src/payload/$(APFPV_GREG_PAYLOAD_PLATFORM_PAYLOAD)/. $(TARGET_DIR)/; \
	fi
	rm -f $(TARGET_DIR)/lib/modules/4.9.84/sigmastar/sensor_imx415_mipi.ko
endef

$(eval $(generic-package))
