
define RUBYFPV_EXTRACT_CMDS
	cp -avr $(RUBYFPV_PKGDIR)/src/* $(@D)/
endef

define RUBYFPV_BUILD_CMDS
	(cd $(@D); $(TARGET_CC) -s ruby_start.c -o ruby_start)
	(cd $(@D); $(TARGET_CC) -s ruby_start.c -o ruby_initradio)
	(cd $(@D); $(TARGET_CC) -s ruby_start.c -o ruby_logger)
	(cd $(@D); $(TARGET_CC) -s ruby_start.c -o ruby_timeinit)
	(cd $(@D); $(TARGET_CC) -s ruby_start.c -o ruby_rt_vehicle)
	(cd $(@D); $(TARGET_CC) -s ruby_start.c -o ruby_tx_telementry)
	(cd $(@D); $(TARGET_CC) -s ruby_start.c -o ruby_rx_rc)
	(cd $(@D); $(TARGET_CC) -s ruby_start.c -o ruby_rx_commands)
	(cd $(@D); $(TARGET_CC) -s ruby_start.c -o ruby_vehicle)
endef

define RUBYFPV_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	cp $(RUBYFPV_PKGDIR)/files/S95ruby $(TARGET_DIR)/etc/init.d

	install -m 0755 -D $(@D)/ruby_start $(TARGET_DIR)/usr/sbin/ruby_start
	install -m 0755 -D $(@D)/ruby_initradio $(TARGET_DIR)/usr/sbin/ruby_initradio
	install -m 0755 -D $(@D)/ruby_logger $(TARGET_DIR)/usr/sbin/ruby_logger
	install -m 0755 -D $(@D)/ruby_timeinit $(TARGET_DIR)/usr/sbin/ruby_timeinit
	install -m 0755 -D $(@D)/ruby_rt_vehicle $(TARGET_DIR)/usr/sbin/ruby_rt_vehicle
	install -m 0755 -D $(@D)/ruby_tx_telemetry $(TARGET_DIR)/usr/sbin/ruby_tx_telemetry
	install -m 0755 -D $(@D)/ruby_rx_rc $(TARGET_DIR)/usr/sbin/ruby_rx_rc
	install -m 0755 -D $(@D)/ruby_rx_commands $(TARGET_DIR)/usr/sbin/ruby_rx_commands
	install -m 0755 -D $(@D)/ruby_vehicle $(TARGET_DIR)/usr/sbin/ruby_vehicle

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	cp $(RUBYFPV_PKGDIR)/files/stop_vehicle.sh $(TARGET_DIR)/usr/sbin 

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	cp $(RUBYFPV_PKGDIR)/files/tweaksys $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
