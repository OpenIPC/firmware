################################################################################
#
# zerotier-one
#
################################################################################

ZEROTIER_ONE_VERSION = 7e46c8359231b4aa55c5d739462c4e5682d35755
ZEROTIER_ONE_SITE = $(call github,zerotier,ZeroTierOne,$(ZEROTIER_ONE_VERSION))
ZEROTIER_ONE_DEPENDENCIES += strace
ZEROTIER_ONE_LICENSE = BUSL-1.1
ZEROTIER_ONE_LICENSE_FILES = LICENSE.txt

ZEROTIER_ONE_MAKE_OPTS = ZT_STATIC=1 ZT_SSO_SUPPORTED=0 \
	CC="$(TARGET_CC)" \
	CXX="$(TARGET_CXX)"

define ZEROTIER_ONE_BUILD_CMDS
	$(MAKE) $(ZEROTIER_ONE_MAKE_OPTS) -C $(@D) all
endef

define ZEROTIER_ONE_INSTALL_TARGET_CMDS
	$(MAKE) -C $(@D) DESTDIR=$(TARGET_DIR) install

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	cp ../general/package/zerotier-one/files/zerotier.conf $(TARGET_DIR)/etc

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	cp ../general/package/zerotier-one/files/S90zerotier $(TARGET_DIR)/etc/init.d
endef

$(eval $(generic-package))
