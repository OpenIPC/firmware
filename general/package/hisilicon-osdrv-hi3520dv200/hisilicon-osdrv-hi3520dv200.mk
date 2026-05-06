################################################################################
#
# hisilicon-osdrv-hi3520dv200
#
################################################################################

HISILICON_OSDRV_HI3520DV200_VERSION =
HISILICON_OSDRV_HI3520DV200_SITE =
HISILICON_OSDRV_HI3520DV200_LICENSE = MIT
HISILICON_OSDRV_HI3520DV200_LICENSE_FILES = LICENSE
HISILICON_OSDRV_HI3520DV200_DEPENDENCIES = uclibc-compat

# Blob install steps are guarded so the build does not fail before the
# vendor .ko/.so are dropped under files/kmod and files/lib. The package
# always installs the load_hisilicon script regardless.
define HISILICON_OSDRV_HI3520DV200_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.0.8/hisilicon
	if ls $(HISILICON_OSDRV_HI3520DV200_PKGDIR)/files/kmod/*.ko >/dev/null 2>&1; then \
		$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon \
			$(HISILICON_OSDRV_HI3520DV200_PKGDIR)/files/kmod/*.ko; \
	fi
	if ls $(HISILICON_OSDRV_HI3520DV200_PKGDIR)/files/kmod/extdrv/*.ko >/dev/null 2>&1; then \
		$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.0.8/hisilicon \
			$(HISILICON_OSDRV_HI3520DV200_PKGDIR)/files/kmod/extdrv/*.ko; \
	fi

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/lib
	if ls $(HISILICON_OSDRV_HI3520DV200_PKGDIR)/files/lib/* >/dev/null 2>&1; then \
		$(INSTALL) -m 644 -t $(TARGET_DIR)/usr/lib \
			$(HISILICON_OSDRV_HI3520DV200_PKGDIR)/files/lib/*; \
	fi

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(HISILICON_OSDRV_HI3520DV200_PKGDIR)/files/script/*
endef

$(eval $(generic-package))
