include $(BR2_EXTERNAL_INGENIC_PATH)/linux/linux-ext-ingenic_patcher.mk
include $(sort $(wildcard $(BR2_EXTERNAL_INGENIC_PATH)/package/*/*.mk))
