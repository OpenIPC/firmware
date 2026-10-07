################################################################################
#
# wpa_supplicant-openipc
#
################################################################################

WPA_SUPPLICANT_OPENIPC_VERSION = 2.11
WPA_SUPPLICANT_OPENIPC_SITE = https://w1.fi/releases
WPA_SUPPLICANT_OPENIPC_SOURCE = wpa_supplicant-$(WPA_SUPPLICANT_OPENIPC_VERSION).tar.gz
WPA_SUPPLICANT_OPENIPC_LICENSE = BSD-3-Clause
WPA_SUPPLICANT_OPENIPC_LICENSE_FILES = README
WPA_SUPPLICANT_OPENIPC_CPE_ID_VENDOR = w1.fi
WPA_SUPPLICANT_OPENIPC_CPE_ID_PRODUCT = wpa_supplicant
WPA_SUPPLICANT_OPENIPC_DEPENDENCIES = host-pkgconf libnl mbedtls-openipc
WPA_SUPPLICANT_OPENIPC_SUBDIR = wpa_supplicant
WPA_SUPPLICANT_OPENIPC_CONFIG = $(@D)/$(WPA_SUPPLICANT_OPENIPC_SUBDIR)/.config
WPA_SUPPLICANT_OPENIPC_CFLAGS = $(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include/libnl3/

# A station on a camera: WPA2-PSK and WPA3-SAE over nl80211 (wext for the
# older vendor drivers), control socket for wpa_cli. No EAP, no AP/P2P/mesh,
# no WPS/DPP, and no TDLS/802.11r (a fixed camera does not roam). Crypto is the
# mbedtls backend from OpenWrt's hostapd patches, on top of mbedtls-openipc.
# (nl80211, wext and the control interface are already on in the upstream
# defconfig; the sed below matches key prefixes, so they are not repeated here)
WPA_SUPPLICANT_OPENIPC_CONFIG_ENABLE = \
	CONFIG_LIBNL32 \
	CONFIG_IEEE80211W \
	CONFIG_SAE \
	CONFIG_OWE \
	CONFIG_MATCH_IFACE \
	CONFIG_NO_RANDOM_POOL \
	CONFIG_NO_CONFIG_WRITE \
	CONFIG_NO_ROAMING

# CONFIG_IEEE8021X_EAPOL stays on with every EAP method off: the PMKSA cache
# is compiled out without it, and drivers that do SAE through external auth
# (rtl8188fu among them) need the PMKID installed after SAE or the association
# is rejected.
WPA_SUPPLICANT_OPENIPC_CONFIG_DISABLE = \
	CONFIG_EAP \
	CONFIG_FILS \
	CONFIG_DRIVER_WIRED \
	CONFIG_MACSEC \
	CONFIG_DRIVER_MACSEC_LINUX \
	CONFIG_HS20 \
	CONFIG_INTERWORKING \
	CONFIG_AP \
	CONFIG_P2P \
	CONFIG_WIFI_DISPLAY \
	CONFIG_MESH \
	CONFIG_HT_OVERRIDES \
	CONFIG_VHT_OVERRIDES \
	CONFIG_HE_OVERRIDES \
	CONFIG_WPS \
	CONFIG_SMARTCARD \
	CONFIG_TDLS \
	CONFIG_IEEE80211R \
	CONFIG_DPP \
	CONFIG_CTRL_IFACE_DBUS_NEW \
	CONFIG_CTRL_IFACE_DBUS_INTRO \
	CONFIG_IBSS_RSN \
	CONFIG_IEEE80211AX \
	CONFIG_SAE_PK \
	CONFIG_DEBUG_SYSLOG

WPA_SUPPLICANT_OPENIPC_CONFIG_EDITS = 's/\#\(CONFIG_TLS=\).*/\1mbedtls/'

define WPA_SUPPLICANT_OPENIPC_CONFIGURE_CMDS
	cp $(@D)/wpa_supplicant/defconfig $(WPA_SUPPLICANT_OPENIPC_CONFIG)
	sed -i $(patsubst %,-e 's/^#\(%\)/\1/',$(WPA_SUPPLICANT_OPENIPC_CONFIG_ENABLE)) \
		$(patsubst %,-e 's/^\(%\)/#\1/',$(WPA_SUPPLICANT_OPENIPC_CONFIG_DISABLE)) \
		$(patsubst %,-e %,$(WPA_SUPPLICANT_OPENIPC_CONFIG_EDITS)) \
		$(WPA_SUPPLICANT_OPENIPC_CONFIG)
	# options not listed in the upstream defconfig
	for s in $(WPA_SUPPLICANT_OPENIPC_CONFIG_ENABLE) ; do \
		if ! grep -q "^$${s}" $(WPA_SUPPLICANT_OPENIPC_CONFIG); then \
			echo "$${s}=y" >> $(WPA_SUPPLICANT_OPENIPC_CONFIG) ; \
		fi \
	done
endef

# LIBS for wpa_supplicant, LIBS_c for wpa_cli, LIBS_p for wpa_passphrase
define WPA_SUPPLICANT_OPENIPC_BUILD_CMDS
	$(TARGET_MAKE_ENV) CFLAGS="$(WPA_SUPPLICANT_OPENIPC_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" BINDIR=/usr/sbin \
		LIBS="$(WPA_SUPPLICANT_OPENIPC_LIBS)" LIBS_c="$(WPA_SUPPLICANT_OPENIPC_LIBS)" \
		LIBS_p="$(WPA_SUPPLICANT_OPENIPC_LIBS)" \
		$(MAKE) CC="$(TARGET_CC)" -C $(@D)/$(WPA_SUPPLICANT_OPENIPC_SUBDIR)
endef

ifeq ($(BR2_PACKAGE_WPA_SUPPLICANT_OPENIPC_CLI),y)
define WPA_SUPPLICANT_OPENIPC_INSTALL_CLI
	$(INSTALL) -m 0755 -D $(@D)/$(WPA_SUPPLICANT_OPENIPC_SUBDIR)/wpa_cli \
		$(TARGET_DIR)/usr/sbin/wpa_cli
endef
endif

ifeq ($(BR2_PACKAGE_WPA_SUPPLICANT_OPENIPC_PASSPHRASE),y)
define WPA_SUPPLICANT_OPENIPC_INSTALL_PASSPHRASE
	$(INSTALL) -m 0755 -D $(@D)/$(WPA_SUPPLICANT_OPENIPC_SUBDIR)/wpa_passphrase \
		$(TARGET_DIR)/usr/sbin/wpa_passphrase
endef
endif

define WPA_SUPPLICANT_OPENIPC_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/$(WPA_SUPPLICANT_OPENIPC_SUBDIR)/wpa_supplicant \
		$(TARGET_DIR)/usr/sbin/wpa_supplicant
	$(WPA_SUPPLICANT_OPENIPC_INSTALL_CLI)
	$(WPA_SUPPLICANT_OPENIPC_INSTALL_PASSPHRASE)
endef

$(eval $(generic-package))
