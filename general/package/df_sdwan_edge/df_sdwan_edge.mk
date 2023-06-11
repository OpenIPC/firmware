################################################################################
#
# DataFlow SDWAN Edge
#
################################################################################

# - Package information
DF_SDWAN_EDGE_VERSION = current
DF_SDWAN_EDGE_SOURCE = df_sdwan_edge.$(OPENIPC_SOC_FAMILY).tar.xz
DF_SDWAN_EDGE_SITE = http://getdataflow.ru/download/sdwan/openipc

DF_SDWAN_EDGE_LICENSE = PROPRIETARY
DF_SDWAN_EDGE_LICENSE_FILES = LICENSE

# - Install commands
define DF_SDWAN_EDGE_INSTALL_TARGET_CMDS
	# --- Create folders
	$(INSTALL) -m 755 -d $(TARGET_DIR)/opt/sdwan
	$(INSTALL) -m 755 -d $(TARGET_DIR)/opt/sdwan/bin
	$(INSTALL) -m 755 -d $(TARGET_DIR)/opt/sdwan/bin/modules
	$(INSTALL) -m 755 -d $(TARGET_DIR)/opt/sdwan/config

	# --- Install binaries
	$(INSTALL) -m 755 $(@D)/bin/sdwan-edge-core $(TARGET_DIR)/opt/sdwan/bin/sdwan-edge-core
	$(INSTALL) -m 644 $(@D)/bin/libdataflow.so $(TARGET_DIR)/opt/sdwan/bin/libdataflow.so
	$(INSTALL) -m 644 $(@D)/bin/modules/df_netio.so $(TARGET_DIR)/opt/sdwan/bin/modules/df_netio.so
	$(INSTALL) -m 644 $(@D)/bin/modules/df_sdfl.so $(TARGET_DIR)/opt/sdwan/bin/modules/df_sdfl.so
	$(INSTALL) -m 644 $(@D)/bin/modules/df_sdwan-edge.so $(TARGET_DIR)/opt/sdwan/bin/modules/df_sdwan-edge.so

	# --- Install configuration
	$(INSTALL) -m 644 $(@D)/config/edge.conf $(TARGET_DIR)/opt/sdwan/config/edge.conf
	$(INSTALL) -m 644 $(@D)/config/memory.conf $(TARGET_DIR)/opt/sdwan/config/memory.conf
	$(INSTALL) -m 644 $(@D)/config/sdwan.conf $(TARGET_DIR)/opt/sdwan/config/sdwan.conf
	$(INSTALL) -m 644 $(@D)/config/sdwan_logger.conf $(TARGET_DIR)/opt/sdwan/config/sdwan_logger.conf

	# --- Install service
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	$(INSTALL) -m 755 $(@D)/service/S45sdwan-edge $(TARGET_DIR)/etc/init.d/S45sdwan-edge
endef

$(eval $(generic-package))
