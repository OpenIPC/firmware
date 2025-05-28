################################################################################
#
# canaan k230 sdk
#
################################################################################
CANAAN_K230_SDK_SITE =
CANAAN_K230_SDK_SOURCE =

CANAAN_K230_SDK_EXTRA_DOWNLOADS :=$(shell \
	if curl --output /dev/null --silent --head --fail https://ai.b-bug.org/~/wangjianxin/ ;then  \
	echo "https://ai.b-bug.org/~/wangjianxin/rootfs.tar.xz";\
	else \
	echo "https://kendryte-download.canaan-creative.com/k230/downloads/kmodel";\
	fi ; )

define CANAAN_K230_SDK_INSTALL_TARGET_CMDS
	tar -xf $(CANAAN_K230_SDK_DL_DIR)/rootfs.tar.xz -C $(TARGET_DIR)
endef



$(eval $(generic-package))
