
define RUBYFPV_EXTRACT_CMDS
	cp -avr $(RUBYFPV_PKGDIR)/src/* $(@D)/
endef

define RUBYFPV_BUILD_CMDS
	(cd $(@D); $(TARGET_CC) -s ruby_start.c -o ruby_start)
endef

define RUBYFPV_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/etc/init.d
	cp $(RUBYFPV_PKGDIR)/files/S95ruby $(TARGET_DIR)/etc/init.d

	install -m 0755 -D $(@D)/ruby_start $(TARGET_DIR)/usr/sbin/ruby_start

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/sbin
	cp $(RUBYFPV_PKGDIR)/files/stop_vehicle.sh $(TARGET_DIR)/usr/sbin 

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	cp $(RUBYFPV_PKGDIR)/files/tweaksys $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
