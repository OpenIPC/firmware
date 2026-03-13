################################################################################
#
# aalink
#
################################################################################

AALINK_SITE = $(patsubst %/,%,$(AALINK_PKGDIR))
AALINK_SITE_METHOD = local
AALINK_LICENSE = Mixed
AALINK_GIT = $(shell command -v git 2>/dev/null)
AALINK_REPO_BRANCH = main
AALINK_REPO = https://github.com/sickgreg/aalink.git

define AALINK_FETCH_REPO
	test -n "$(AALINK_GIT)" || { \
		echo "git is required to build aalink"; \
		exit 1; \
	}
	rm -rf $(@D)/src
	$(AALINK_GIT) clone --depth 1 --branch $(AALINK_REPO_BRANCH) $(AALINK_REPO) $(@D)/src
endef

AALINK_PRE_CONFIGURE_HOOKS += AALINK_FETCH_REPO

define AALINK_BUILD_CMDS
	$(TARGET_CC) $(TARGET_CFLAGS) -O2 -Wall -Wextra \
		-o $(@D)/aalink \
		$(@D)/src/aalink.c \
		$(TARGET_LDFLAGS) -lpthread -lm
endef

define AALINK_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(@D)/src/aalink.conf
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(AALINK_PKGDIR)/files/S991aalink
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/aalink
endef

$(eval $(generic-package))
