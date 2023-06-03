export OPENIPC_VENDOR = $(call qstrip,$(BR2_OPENIPC_VENDOR))
export OPENIPC_MODEL = $(call qstrip,$(BR2_OPENIPC_MODEL))
export OPENIPC_FAMILY = $(call qstrip,$(BR2_OPENIPC_FAMILY))
export OPENIPC_RELEASE = $(call qstrip,$(BR2_OPENIPC_RELEASE))

export BR2_EXTERNAL_VENDOR = $(BR2_EXTERNAL)/../br-ext-chip-$(OPENIPC_VENDOR)
OPENIPC_TOOLCHAIN = latest/$(shell $(BR2_EXTERNAL)/scripts/show_toolchains.sh $(BR2_DEFCONFIG))

include $(sort $(wildcard $(BR2_EXTERNAL)/package/*/*.mk))
