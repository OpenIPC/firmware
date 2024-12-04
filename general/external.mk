export OPENIPC_SOC_VENDOR := $(call qstrip,$(BR2_OPENIPC_SOC_VENDOR))
export OPENIPC_SOC_MODEL := $(call qstrip,$(BR2_OPENIPC_SOC_MODEL))
export OPENIPC_SOC_FAMILY := $(call qstrip,$(BR2_OPENIPC_SOC_FAMILY))
export OPENIPC_SNS_MODEL := $(call qstrip,$(BR2_OPENIPC_SNS_MODEL))
export OPENIPC_VARIANT := $(call qstrip,$(BR2_OPENIPC_VARIANT))
export OPENIPC_MAJESTIC := $(call qstrip,$(BR2_OPENIPC_MAJESTIC))
export WGET := wget --show-progress --passive-ftp -nd -t5 -T10

EXTERNAL_VENDOR := $(BR2_EXTERNAL)/../br-ext-chip-$(OPENIPC_SOC_VENDOR)
OPENIPC_KERNEL := $(OPENIPC_SOC_VENDOR)-$(OPENIPC_SOC_FAMILY)
OPENIPC_TOOLCHAIN := toolchain/toolchain.$(OPENIPC_KERNEL)

include $(sort $(wildcard $(BR2_EXTERNAL)/package/*/*.mk))
