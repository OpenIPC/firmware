include $(BR2_EXTERNAL_ALLWINNER_PATH)/linux/linux-ext-allwinner_patcher.mk
include $(sort $(wildcard $(BR2_EXTERNAL_ALLWINNER_PATH)/package/*/*.mk))
