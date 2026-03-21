################################################################################
#
# Copyright (c) OpenIPC  https://openipc.org  MIT License
#
# faceter-agent  2025.12.10 — faceter-agent  2025.12.10 Buildroot package
#
################################################################################

FACETER_AGENT_SITE = https://github.com/OpenIPC/faceter/releases/download/faceter-agent
FACETER_AGENT_SOURCE = faceter-agent.$(FACETER_AGENT_FAMILY).tar.bz2
FACETER_AGENT_LICENSE = The Prosperity Public License 3.0.0
FACETER_AGENT_LICENSE_FILES = LICENSE

FACETER_AGENT_FAMILY = $(OPENIPC_SOC_FAMILY)

FACETER_AGENT_DEPENDENCIES += \
	majestic \
	mbedtls-openipc \
	json-c

define FACETER_AGENT_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc
	$(INSTALL) -m 644 -t $(TARGET_DIR)/etc $(@D)/faceter-agent.conf

	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 -t $(TARGET_DIR)/etc/init.d $(@D)/S96faceter-agent

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/share/faceter
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/share/faceter $(@D)/faceterCA.pem
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/share/faceter $(@D)/openipc_features.json

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(@D)/faceter-agent
endef

$(eval $(generic-package))


