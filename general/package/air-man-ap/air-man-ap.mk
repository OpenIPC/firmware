################################################################################
#
# air-man-ap
#
################################################################################

AIR_MAN_AP_SITE = $(patsubst %/,%,$(AIR_MAN_AP_PKGDIR))
AIR_MAN_AP_SITE_METHOD = local
AIR_MAN_AP_LICENSE = Mixed
AIR_MAN_AP_GIT = $(shell command -v git 2>/dev/null)
AIR_MAN_AP_REPO_BRANCH = main
AIR_MAN_AP_REPO = https://github.com/sickgreg/air_man_ap.git

define AIR_MAN_AP_FETCH_REPO
	test -n "$(AIR_MAN_AP_GIT)" || { \
		echo "git is required to build air-man-ap"; \
		exit 1; \
	}
	rm -rf $(@D)/src
	$(AIR_MAN_AP_GIT) clone --depth 1 --branch $(AIR_MAN_AP_REPO_BRANCH) $(AIR_MAN_AP_REPO) $(@D)/src
endef

AIR_MAN_AP_PRE_CONFIGURE_HOOKS += AIR_MAN_AP_FETCH_REPO

define AIR_MAN_AP_BUILD_CMDS
	$(TARGET_CC) $(TARGET_CFLAGS) -O2 -Wall -Wextra -pthread \
		-o $(@D)/air_man_ap \
		$(@D)/src/air_man_ap.c \
		$(TARGET_LDFLAGS)
endef

define AIR_MAN_AP_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(AIR_MAN_AP_PKGDIR)/files/S992air_man
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/sensors
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(@D)/src/modes_imx335.ini
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc/sensors $(@D)/src/modes_imx415.ini
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/air_man_ap
endef

$(eval $(generic-package))
