################################################################################
#
# xiongmai-osdrv-xm530v200
#
################################################################################

XIONGMAI_OSDRV_XM530V200_VERSION =
XIONGMAI_OSDRV_XM530V200_SITE =
XIONGMAI_OSDRV_XM530V200_LICENSE = PROPRIETARY

# The V200 series' vendor modules, one set per chip, from camera firmware
# (OpenIPC/qemu-bench#154):
#   xm530v200  X6C-WEQ, V5.04.R02.20241125
#   xm533      X3-WQ-B, V5.04.R02.20250828
#   xm535d1    X6E-WEQ, V5.07.X02.20250906
#   xm535d2    X9-WE2Q, V5.02.X02.20251223 (the same media modules as the D1)
#   xm535w1    X2W-WNG, V5.00.R02.20240410
# They are built for their 3.10.103 vendor kernels. This family's kernel
# matches their vermagic (SMP on xm530v200, UP on the others) and exports
# what those kernels add (clk_get_pts, kthread_set_prio).
XIONGMAI_OSDRV_XM530V200_FILES = $(XIONGMAI_OSDRV_XM530V200_PKGDIR)/files/$(OPENIPC_SOC_MODEL)

# xiongmai-opensdk-xm530v200 builds open_mmz.ko, open_sys.ko and open_vb.ko
# in place of mmz.ko, sys.ko and vb.ko. They export the same symbols, and depmod would make the
# modules that need them depend on the vendor ones. So those move out of the
# tree depmod reads, to where load_xiongmai finds them only when the open
# ones do not load.
ifeq ($(BR2_PACKAGE_XIONGMAI_OPENSDK_XM530V200),y)
define XIONGMAI_OSDRV_XM530V200_DROP_REPLACED
	for m in mmz sys vb rgn; do \
		$(INSTALL) -D -m 644 $(TARGET_DIR)/lib/modules/3.10.103+/xiongmai/$$m.ko \
			$(TARGET_DIR)/usr/lib/xiongmai/$$m.ko && \
		rm -f $(TARGET_DIR)/lib/modules/3.10.103+/xiongmai/$$m.ko || exit 1; \
	done
endef
endif

define XIONGMAI_OSDRV_XM530V200_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.10.103+/xiongmai
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.103+/xiongmai $(XIONGMAI_OSDRV_XM530V200_FILES)/kmod/*.ko
	$(XIONGMAI_OSDRV_XM530V200_DROP_REPLACED)

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(XIONGMAI_OSDRV_XM530V200_FILES)/load_xiongmai
endef

$(eval $(generic-package))
