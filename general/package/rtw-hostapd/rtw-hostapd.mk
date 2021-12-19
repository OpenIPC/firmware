################################################################################
#
# hostapd
#
################################################################################

RTW_HOSTAPD_VERSION = a69d6361ef0185aa7d2e4c774bc2de36fe83d81e
RTW_HOSTAPD_SITE = $(call github,lwfinger,rtl8188eu,$(RTW_HOSTAPD_VERSION))
RTW_HOSTAPD_SUBDIR = hostapd-2.9/hostapd
RTW_HOSTAPD_CONFIG = $(RTW_HOSTAPD_DIR)/$(RTW_HOSTAPD_SUBDIR)/.config
#RTW_HOSTAPD_PATCH = \
#	https://w1.fi/security/2020-1/0001-WPS-UPnP-Do-not-allow-event-subscriptions-with-URLs-.patch \
#	https://w1.fi/security/2020-1/0002-WPS-UPnP-Fix-event-message-generation-using-a-long-U.patch \
#	https://w1.fi/security/2020-1/0003-WPS-UPnP-Handle-HTTP-initiation-failures-for-events-.patch
RTW_HOSTAPD_DEPENDENCIES = host-pkgconf
RTW_HOSTAPD_CFLAGS = $(TARGET_CFLAGS)
RTW_HOSTAPD_LICENSE = BSD-3-Clause
RTW_HOSTAPD_LICENSE_FILES = README

# 0001-AP-Silently-ignore-management-frame-from-unexpected-.patch
RTW_HOSTAPD_IGNORE_CVES += CVE-2019-16275

# 0001-WPS-UPnP-Do-not-allow-event-subscriptions-with-URLs-.patch
RTW_HOSTAPD_IGNORE_CVES += CVE-2020-12695

RTW_HOSTAPD_CONFIG_SET =

RTW_HOSTAPD_CONFIG_ENABLE = \
	CONFIG_INTERNAL_LIBTOMMATH \
	CONFIG_DEBUG_FILE \
	CONFIG_DEBUG_SYSLOG

RTW_HOSTAPD_CONFIG_DISABLE =

# Try to use openssl if it's already available
ifeq ($(BR2_PACKAGE_LIBOPENSSL),y)
RTW_HOSTAPD_DEPENDENCIES += host-pkgconf libopenssl
RTW_HOSTAPD_LIBS += `$(PKG_CONFIG_HOST_BINARY) --libs openssl`
RTW_HOSTAPD_CONFIG_EDITS += 's/\#\(CONFIG_TLS=openssl\)/\1/'
else
RTW_HOSTAPD_CONFIG_DISABLE += CONFIG_EAP_PWD CONFIG_EAP_TEAP
RTW_HOSTAPD_CONFIG_EDITS += 's/\#\(CONFIG_TLS=\).*/\1internal/'
endif

ifeq ($(BR2_PACKAGE_RTW_HOSTAPD_DRIVER_HOSTAP),)
RTW_HOSTAPD_CONFIG_DISABLE += CONFIG_DRIVER_HOSTAP
endif

ifeq ($(BR2_PACKAGE_RTW_HOSTAPD_DRIVER_NL80211),y)
RTW_HOSTAPD_CONFIG_ENABLE += CONFIG_DRIVER_NL80211
endif

ifeq ($(BR2_PACKAGE_RTW_HOSTAPD_DRIVER_RTW),)
RTW_HOSTAPD_CONFIG_DISABLE += CONFIG_DRIVER_RTW
endif

ifeq ($(BR2_PACKAGE_RTW_HOSTAPD_DRIVER_WIRED),y)
RTW_HOSTAPD_CONFIG_ENABLE += CONFIG_DRIVER_WIRED
endif

ifeq ($(BR2_PACKAGE_RTW_HOSTAPD_DRIVER_NONE),y)
RTW_HOSTAPD_CONFIG_ENABLE += CONFIG_DRIVER_NONE
endif

# Add options for wireless drivers
ifeq ($(BR2_PACKAGE_RTW_HOSTAPD_HAS_WIFI_DRIVERS),y)
RTW_HOSTAPD_CONFIG_ENABLE += \
	CONFIG_HS20 \
	CONFIG_IEEE80211AC \
	CONFIG_IEEE80211N \
	CONFIG_IEEE80211R \
	CONFIG_INTERWORKING
endif

ifeq ($(BR2_PACKAGE_RTW_HOSTAPD_ACS),y)
RTW_HOSTAPD_CONFIG_ENABLE += CONFIG_ACS
endif

ifeq ($(BR2_PACKAGE_RTW_HOSTAPD_EAP),y)
RTW_HOSTAPD_CONFIG_ENABLE += \
	CONFIG_EAP \
	CONFIG_RADIUS_SERVER

# Enable both TLS v1.1 (CONFIG_TLSV11) and v1.2 (CONFIG_TLSV12)
RTW_HOSTAPD_CONFIG_ENABLE += CONFIG_TLSV1
else
RTW_HOSTAPD_CONFIG_DISABLE += CONFIG_EAP
RTW_HOSTAPD_CONFIG_ENABLE += \
	CONFIG_NO_ACCOUNTING \
	CONFIG_NO_RADIUS
endif

ifeq ($(BR2_PACKAGE_RTW_HOSTAPD_WPS),y)
RTW_HOSTAPD_CONFIG_ENABLE += CONFIG_WPS
endif

ifeq ($(BR2_PACKAGE_RTW_HOSTAPD_WPA3),y)
RTW_HOSTAPD_CONFIG_SET += \
	CONFIG_DPP \
	CONFIG_SAE
RTW_HOSTAPD_CONFIG_ENABLE += \
	CONFIG_OWE
else
RTW_HOSTAPD_CONFIG_DISABLE += \
	CONFIG_OWE
endif

ifeq ($(BR2_PACKAGE_RTW_HOSTAPD_VLAN),)
RTW_HOSTAPD_CONFIG_ENABLE += CONFIG_NO_VLAN
endif

ifeq ($(BR2_PACKAGE_RTW_HOSTAPD_VLAN_DYNAMIC),y)
RTW_HOSTAPD_CONFIG_ENABLE += CONFIG_FULL_DYNAMIC_VLAN
RTW_HOSTAPD_CONFIG_SET += NEED_LINUX_IOCTL
endif

ifeq ($(BR2_PACKAGE_RTW_HOSTAPD_VLAN_NETLINK),y)
RTW_HOSTAPD_CONFIG_ENABLE += CONFIG_VLAN_NETLINK
endif

# Options for building with libnl
ifeq ($(BR2_PACKAGE_LIBNL),y)
RTW_HOSTAPD_DEPENDENCIES += libnl
RTW_HOSTAPD_CFLAGS += -I$(STAGING_DIR)/usr/include/libnl3/
RTW_HOSTAPD_CONFIG_ENABLE += CONFIG_LIBNL32
# libnl-3 needs -lm (for rint) and -lpthread if linking statically
# And library order matters hence stick -lnl-3 first since it's appended
# in the hostapd Makefiles as in LIBS+=-lnl-3 ... thus failing
ifeq ($(BR2_STATIC_LIBS),y)
RTW_HOSTAPD_LIBS += -lnl-3 -lm -lpthread
endif
endif

define RTW_HOSTAPD_CONFIGURE_CMDS
	cp $(@D)/$(RTW_HOSTAPD_SUBDIR)/defconfig $(RTW_HOSTAPD_CONFIG)
	sed -i $(patsubst %,-e 's/^#\(%\)/\1/',$(RTW_HOSTAPD_CONFIG_ENABLE)) \
		$(patsubst %,-e 's/^\(%\)/#\1/',$(RTW_HOSTAPD_CONFIG_DISABLE)) \
		$(patsubst %,-e '1i%=y',$(RTW_HOSTAPD_CONFIG_SET)) \
		$(patsubst %,-e %,$(RTW_HOSTAPD_CONFIG_EDITS)) \
		$(RTW_HOSTAPD_CONFIG)
endef

define RTW_HOSTAPD_BUILD_CMDS
	$(MAKE) -C $(@D)/$(RTW_HOSTAPD_SUBDIR) clean
	$(TARGET_MAKE_ENV) CFLAGS="$(RTW_HOSTAPD_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" LIBS="$(RTW_HOSTAPD_LIBS)" \
		$(MAKE) CC="$(TARGET_CC)" -C $(@D)/$(RTW_HOSTAPD_SUBDIR)
endef

define RTW_HOSTAPD_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/$(RTW_HOSTAPD_SUBDIR)/hostapd \
		$(TARGET_DIR)/usr/sbin/hostapd
	$(INSTALL) -m 0755 -D $(@D)/$(RTW_HOSTAPD_SUBDIR)/hostapd_cli \
		$(TARGET_DIR)/usr/bin/hostapd_cli
	$(INSTALL) -m 0644 -D $(@D)/$(RTW_HOSTAPD_SUBDIR)/hostapd.conf \
		$(TARGET_DIR)/etc/hostapd.conf
endef
$(eval $(generic-package))
