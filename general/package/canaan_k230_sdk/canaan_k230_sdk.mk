################################################################################
#
# canaan k230 sdk
#
################################################################################
CANAAN_K230_SDK_SITE =
CANAAN_K230_SDK_SOURCE =

k230_download_url := $(shell \
	if curl --output /dev/null --silent --head --fail https://ai.b-bug.org/~/wangjianxin/ ;then  \
	echo "https://ai.b-bug.org/~/wangjianxin/"; \
	else \
	echo "https://kendryte-download.canaan-creative.com/k230/downloads/kmodel";\
	fi ; )


CANAAN_K230_SDK_EXTRA_DOWNLOADS := $(k230_download_url)/sysimage-sdcard.img.gz
CANAAN_K230_SDK_EXTRA_DOWNLOADS += $(k230_download_url)/rootfs.tar.xz

define CANAAN_K230_SDK_INSTALL_TARGET_CMDS
	tar -xf $(CANAAN_K230_SDK_DL_DIR)/rootfs.tar.xz -C $(TARGET_DIR)
	rm -rf $(TARGET_DIR)/var/run;
	gunzip -f -k  -c $(CANAAN_K230_SDK_DL_DIR)/sysimage-sdcard.img.gz > $(BINARIES_DIR)/sysimage-sdcard.img
endef



$(eval $(generic-package))
