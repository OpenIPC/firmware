################################################################################
#
# Adaptive Link
#
################################################################################

YAML_CLI_MULTI_SITE = $(call github,vertexodessa,yaml-cli-multi,$(YAML_CLI_MULTI_VERSION))
YAML_CLI_MULTI_VERSION = 'a633865b5202dcd1f4f548e8486d3a334c79c03c'

YAML_CLI_MULTI_LICENSE = GPL-3.0
YAML_CLI_MULTI_LICENSE_FILES = LICENSE


ifeq ($(BR2_TOOLCHAIN_USES_GLIBC),y)
	YAML_CLI_MULTI_OPTIONS = "-rdynamic -s -Os -lm"
else
	YAML_CLI_MULTI_OPTIONS = "-rdynamic -s -Os"
endif

define YAML_CLI_MULTI_BUILD_CMDS
	@echo "Building yaml-cli-multi"
	$(MAKE) CC=$(TARGET_CC) OPT=$(YAML_CLI_MULTI_OPTIONS) -C $(@D)
endef

define YAML_CLI_MULTI_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin

	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/yaml-cli-multi
endef

$(eval $(generic-package))
