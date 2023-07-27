export OPENIPC_SOC_VENDOR := $(call qstrip,$(BR2_OPENIPC_SOC_VENDOR))
export OPENIPC_SOC_MODEL := $(call qstrip,$(BR2_OPENIPC_SOC_MODEL))
export OPENIPC_SOC_FAMILY := $(call qstrip,$(BR2_OPENIPC_SOC_FAMILY))
export OPENIPC_FLAVOR := $(call qstrip,$(BR2_OPENIPC_FLAVOR))

EXTERNAL_VENDOR := $(BR2_EXTERNAL)/../br-ext-chip-$(OPENIPC_SOC_VENDOR)
OPENIPC_TOOLCHAIN := latest/$(shell $(BR2_EXTERNAL)/scripts/show_toolchains.sh $(BR2_DEFCONFIG))

ifeq ($(OPENIPC_SOC_FAMILY),t21)
	BR2_OPENIPC_KERNEL := $(OPENIPC_SOC_VENDOR)-t31
else
	BR2_OPENIPC_KERNEL := $(OPENIPC_SOC_VENDOR)-$(OPENIPC_SOC_FAMILY)
endif

ifeq ($(call qstrip,$(BR2_DL_DIR)),$(TOPDIR)/dl)
	OPENIPC_KERNEL := $(BR2_OPENIPC_KERNEL)
else
	OPENIPC_KERNEL := $(shell git ls-remote https://github.com/openipc/linux \
		$(BR2_OPENIPC_KERNEL) | head -1 | cut -f1)
	LOCAL_DOWNLOAD = y
endif

include $(sort $(wildcard $(BR2_EXTERNAL)/package/*/*.mk))
