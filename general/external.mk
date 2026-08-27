export OPENIPC_SOC_VENDOR := $(call qstrip,$(BR2_OPENIPC_SOC_VENDOR))
export OPENIPC_SOC_MODEL := $(call qstrip,$(BR2_OPENIPC_SOC_MODEL))
export OPENIPC_SOC_ALIASES := $(call qstrip,$(BR2_OPENIPC_SOC_ALIASES))
export OPENIPC_SOC_FAMILY := $(call qstrip,$(BR2_OPENIPC_SOC_FAMILY))
export OPENIPC_SNS_MODEL := $(call qstrip,$(BR2_OPENIPC_SNS_MODEL))
export OPENIPC_VARIANT := $(call qstrip,$(BR2_OPENIPC_VARIANT))
export OPENIPC_MAJESTIC := $(call qstrip,$(BR2_OPENIPC_MAJESTIC))
export WGET := wget --show-progress --passive-ftp -nd -t5 -T10

EXTERNAL_VENDOR := $(BR2_EXTERNAL)/../br-ext-chip-$(OPENIPC_SOC_VENDOR)
OPENIPC_KERNEL := $(OPENIPC_SOC_VENDOR)-$(OPENIPC_SOC_FAMILY)
OPENIPC_TOOLCHAIN := toolchain/toolchain.$(OPENIPC_KERNEL)

# Buildroot leaves upstream's wpa_supplicant defconfig defaults for TDLS and
# 802.11r in place and offers no Kconfig switch for either, so every board that
# enables wpa_supplicant carries both. Neither can fire on a camera: TDLS sets
# up direct station-to-station data links, and a camera only ever talks to its
# AP; 802.11r is fast BSS transition, and a fixed-mount camera does not roam
# between APs. Measured on gk7205v300_lite -- 28,860 and 21,940 bytes of the
# unstripped binary, 44,976 off the stripped one.
#
# Appending to the package's own list rather than patching its defconfig works
# because Buildroot includes this file (Makefile:545) after package/*/*.mk
# (Makefile:531), and expands WPA_SUPPLICANT_CONFIGURE_CMDS when the rule runs
# rather than at parse time. That leaves nothing to rebase when the package is
# bumped. If a future Buildroot ever reorders those includes this stops taking
# effect silently, and the symptom is the wpa_supplicant binary going back up
# by ~45KB in the size report rather than anything failing.
WPA_SUPPLICANT_CONFIG_DISABLE += CONFIG_TDLS CONFIG_IEEE80211R

include $(sort $(wildcard $(BR2_EXTERNAL)/package/*/*.mk))
include $(sort $(wildcard $(BR2_EXTERNAL)/package/legacy/*/*.mk))
