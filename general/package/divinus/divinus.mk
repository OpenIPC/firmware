################################################################################
#
# divinus
#
################################################################################

DIVINUS_SITE = $(call github,openipc,divinus,$(DIVINUS_VERSION))
DIVINUS_VERSION = HEAD
DIVINUS_LICENSE = MIT
DIVINUS_LICENSE_FILES = LICENSE

ifeq ($(BR2_TOOLCHAIN_USES_GLIBC),y)
	DIVINUS_OPTIONS = "-rdynamic -s -Os -lm"
else ifeq ($(BR2_PACKAGE_SIGMASTAR_OSDRV_MSC313E),y)
	# The infinity3 SDK libraries are glibc builds: link the shim ahead of musl
	# (see glibc-compat/Config.in). Their threads also expect glibc's large
	# default stack; musl's 128 KiB one overflows in the camera pipeline.
	DIVINUS_DEPENDENCIES += glibc-compat
	DIVINUS_OPTIONS = "-rdynamic -s -Os -DGLIBC_COMPAT -Wl,-z,stack-size=1048576 -Wl,--no-as-needed -lglibc-compat"
else
	DIVINUS_OPTIONS = "-rdynamic -s -Os"
endif

define DIVINUS_BUILD_CMDS
	$(MAKE) CC=$(TARGET_CC) OPT=$(DIVINUS_OPTIONS) -C $(@D)/src
endef

define DIVINUS_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(@D)/divinus.yaml

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/divinus
endef

$(eval $(generic-package))
