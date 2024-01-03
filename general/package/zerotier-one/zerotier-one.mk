################################################################################
#
# zerotier-one
#
################################################################################

ZEROTIER_ONE_VERSION = 1.12.2
ZEROTIER_ONE_SITE = $(call github,zerotier,ZeroTierOne,$(ZEROTIER_ONE_VERSION))

ZEROTIER_ONE_LICENSE = BUSL-1.1
ZEROTIER_ONE_LICENSE_FILES = LICENSE.txt

ZEROTIER_ONE_MAKE_OPTS = ZT_STATIC=1 ZT_SSO_SUPPORTED=0 \
	CC="$(TARGET_CC)" \
	CXX="$(TARGET_CXX)" \
	FLOATABI="$(BR2_GCC_TARGET_FLOAT_ABI)"

ZEROTIER_ONE_DEPENDENCIES = \
	libminiupnpc \
	libnatpmp

define ZEROTIER_ONE_BUILD_CMDS
	$(MAKE) $(ZEROTIER_ONE_MAKE_OPTS) -C $(@D) all
endef

define ZEROTIER_ONE_INSTALL_TARGET_CMDS
	$(MAKE) -C $(@D) DESTDIR=$(TARGET_DIR) install

	if grep -q "BR2_PACKAGE_WEBUI=y" $(BR2_CONFIG); then \
		$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d ; \
		cp $(ZEROTIER_ONE_PKGDIR)/files/S90zerotier $(TARGET_DIR)/etc/init.d ; \
	fi
endef

$(eval $(generic-package))
