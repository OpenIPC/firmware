export OPENIPC_SOC_VENDOR := $(call qstrip,$(BR2_OPENIPC_SOC_VENDOR))
export OPENIPC_SOC_MODEL := $(call qstrip,$(BR2_OPENIPC_SOC_MODEL))
export OPENIPC_SOC_FAMILY := $(call qstrip,$(BR2_OPENIPC_SOC_FAMILY))
export OPENIPC_SNS_MODEL := $(call qstrip,$(BR2_OPENIPC_SNS_MODEL))
export OPENIPC_VARIANT := $(call qstrip,$(BR2_OPENIPC_VARIANT))
export OPENIPC_MAJESTIC := $(call qstrip,$(BR2_OPENIPC_MAJESTIC))

EXTERNAL_VENDOR := $(BR2_EXTERNAL)/../br-ext-chip-$(OPENIPC_SOC_VENDOR)
OPENIPC_TOOLCHAIN := latest/$(shell $(BR2_EXTERNAL)/scripts/show_toolchains.sh $(BR2_DEFCONFIG))
OPENIPC_KERNEL := $(OPENIPC_SOC_VENDOR)-$(OPENIPC_SOC_FAMILY)

include $(sort $(wildcard $(BR2_EXTERNAL)/package/*/*.mk))
