################################################################################
#
# aalink-idr5
#
################################################################################

AALINK_IDR5_SITE = $(patsubst %/,%,$(AALINK_IDR5_PKGDIR))
AALINK_IDR5_SITE_METHOD = local
AALINK_IDR5_LICENSE = Mixed
AALINK_IDR5_GIT = $(shell command -v git 2>/dev/null)
AALINK_IDR5_REPO_BRANCH = main
AALINK_IDR5_REPO = https://github.com/sickgreg/aalink_idr5.git

define AALINK_IDR5_FETCH_REPO
	test -n "$(AALINK_IDR5_GIT)" || { \
		echo "git is required to build aalink-idr5"; \
		exit 1; \
	}
	rm -rf $(@D)/src
	$(AALINK_IDR5_GIT) clone --depth 1 --branch $(AALINK_IDR5_REPO_BRANCH) $(AALINK_IDR5_REPO) $(@D)/src
endef

AALINK_IDR5_PRE_CONFIGURE_HOOKS += AALINK_IDR5_FETCH_REPO

define AALINK_IDR5_BUILD_CMDS
	$(TARGET_CC) $(TARGET_CFLAGS) -O2 -Wall -Wextra \
		-o $(@D)/aalink_idr5 \
		$(@D)/src/aalink_idr5.c \
		$(TARGET_LDFLAGS)
endef

define AALINK_IDR5_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(AALINK_IDR5_PKGDIR)/files/S94idr
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/aalink_idr5
endef

$(eval $(generic-package))
