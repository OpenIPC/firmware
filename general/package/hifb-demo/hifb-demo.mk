################################################################################
#
# hifb-demo
#
################################################################################

HIFB_DEMO_VERSION = 1.0
HIFB_DEMO_SITE_METHOD = local
HIFB_DEMO_SITE = $(HIFB_DEMO_PKGDIR)
HIFB_DEMO_LICENSE = MIT
HIFB_DEMO_DEPENDENCIES = hisilicon-osdrv-hi3520dv200

HIFB_DEMO_CFLAGS = -Os -Wall -Wno-unused-function \
	-I$(HIFB_DEMO_PKGDIR)/include
HIFB_DEMO_LDFLAGS = -L$(STAGING_DIR)/usr/lib -lmpi \
	-Wl,--unresolved-symbols=ignore-in-shared-libs

define HIFB_DEMO_BUILD_CMDS
	$(TARGET_CC) $(HIFB_DEMO_CFLAGS) -o $(@D)/hifb_demo \
		$(HIFB_DEMO_PKGDIR)/src/hifb_demo.c \
		$(HIFB_DEMO_PKGDIR)/src/sample_comm_sys.c \
		$(HIFB_DEMO_PKGDIR)/src/sample_comm_vo.c \
		$(HIFB_DEMO_LDFLAGS)
endef

define HIFB_DEMO_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -D $(@D)/hifb_demo $(TARGET_DIR)/usr/bin/hifb_demo
endef

$(eval $(generic-package))
