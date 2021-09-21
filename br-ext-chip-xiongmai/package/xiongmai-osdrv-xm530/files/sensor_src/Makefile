TOPDIR = .
include $(TOPDIR)/Configure



DIR_LIBSNS      = $(TOPDIR)/sns$(SENSOR_ID)

ifeq ($(CHIP_ID),XM510)
	MKF_NAME = Makefile_xm510
endif
ifeq ($(CHIP_ID),XM320)
	MKF_NAME = Makefile_xm320
endif

ifeq ($(CHIP_ID),XM330)
	MKF_NAME = Makefile_xm330
endif

ifeq ($(CHIP_ID),XM350)
	MKF_NAME = Makefile_xm350
endif

ifeq ($(CHIP_ID),XM530)
	MKF_NAME = Makefile_xm530
endif

rel:
	@cd $(DIR_LIBSNS);   make -f $(MKF_NAME)

clean:
	@cd $(DIR_LIBSNS);   make clean -f $(MKF_NAME)

