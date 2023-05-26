export OPENIPC_VENDOR = $(call qstrip,$(BR2_OPENIPC_VENDOR))
export OPENIPC_MODEL = $(call qstrip,$(BR2_OPENIPC_MODEL))
export OPENIPC_FAMILY = $(call qstrip,$(BR2_OPENIPC_FAMILY))
export OPENIPC_RELEASE = $(call qstrip,$(BR2_OPENIPC_RELEASE))

export BR2_EXTERNAL_SCRIPTS = $(BR2_EXTERNAL)/scripts
export BR2_EXTERNAL_VENDOR = $(BR2_EXTERNAL)/../br-ext-chip-$(BR2_OPENIPC_VENDOR)

include $(sort $(wildcard $(BR2_EXTERNAL)/package/*/*.mk))
