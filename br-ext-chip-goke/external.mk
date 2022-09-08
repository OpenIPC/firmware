include $(BR2_EXTERNAL_GOKE_PATH)/linux/linux-ext-goke_patcher.mk
include $(sort $(wildcard $(BR2_EXTERNAL_GOKE_PATH)/package/*/*.mk))
