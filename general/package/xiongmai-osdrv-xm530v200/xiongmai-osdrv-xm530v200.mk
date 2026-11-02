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

define XIONGMAI_OSDRV_XM530V200_INSTALL_TARGET_CMDS
	$(INSTALL) -m 755 -d $(TARGET_DIR)/lib/modules/3.10.103+/xiongmai
	$(INSTALL) -m 644 -t $(TARGET_DIR)/lib/modules/3.10.103+/xiongmai $(XIONGMAI_OSDRV_XM530V200_FILES)/kmod/*.ko

	$(INSTALL) -m 755 -d $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 755 -t $(TARGET_DIR)/usr/bin $(XIONGMAI_OSDRV_XM530V200_FILES)/load_xiongmai
endef

$(eval $(generic-package))
