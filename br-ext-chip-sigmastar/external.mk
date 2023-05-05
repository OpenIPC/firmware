include $(BR2_EXTERNAL_SIGMASTAR_PATH)/linux/linux-ext-sigmastar_patcher.mk
include $(sort $(wildcard $(BR2_EXTERNAL_SIGMASTAR_PATH)/../general/package/*/*.mk))
