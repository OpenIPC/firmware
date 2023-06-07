export OPENIPC_SOC_VENDOR = $(call qstrip,$(BR2_OPENIPC_SOC_VENDOR))
export OPENIPC_SOC_MODEL = $(call qstrip,$(BR2_OPENIPC_SOC_MODEL))
export OPENIPC_SOC_FAMILY = $(call qstrip,$(BR2_OPENIPC_SOC_FAMILY))
export OPENIPC_FLAVOR = $(call qstrip,$(BR2_OPENIPC_FLAVOR))

OPENIPC_TOOLCHAIN = latest/$(shell $(BR2_EXTERNAL)/scripts/show_toolchains.sh $(BR2_DEFCONFIG))
EXTERNAL_VENDOR = $(BR2_EXTERNAL)/../br-ext-chip-$(OPENIPC_SOC_VENDOR)

include $(sort $(wildcard $(BR2_EXTERNAL)/package/*/*.mk))
