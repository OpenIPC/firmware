################################################################################
# ap_alink - Buildroot package using upstream Makefile from GitHub
################################################################################

APALINK_VERSION = 1.2
APALINK_SITE = https://github.com/carabidulebabat/AP-ALINK-FPV.git
APALINK_SITE_METHOD = git

# Build (uses the Makefile from the repo)
define APALINK_BUILD_CMDS
	$(MAKE) -C $(@D) \
		CC="$(TARGET_CC)" \
		CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

# Install (binary + optional config file)
define APALINK_INSTALL_TARGET_CMDS
	# Installe le binaire compilé
	$(INSTALL) -D -m 0755 $(@D)/ap_alink $(TARGET_DIR)/usr/bin/ap_alink

	# Si tu ajoutes un fichier de conf dans ton dépôt (ex: ap_alink.conf)
	# décommente cette ligne :
	 $(INSTALL) -D -m 0644 $(@D)/ap_alink.conf $(TARGET_DIR)/etc/ap_alink.conf
endef

$(eval $(generic-package))
