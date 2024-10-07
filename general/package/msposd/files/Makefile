# Get the current date and time in the format YYYYMMDD_HHMMSS
VERSION_STRING := $(shell date +"%Y%m%d_%H%M%S")
CFLAGS = -Wno-address-of-packed-member -DVERSION_STRING="\"$(VERSION_STRING)\""

SRCS := compat.c msposd.c bmp/bitmap.c bmp/region.c bmp/lib/schrift.c bmp/text.c osd/net/network.c osd/msp/msp.c osd/msp/msp_displayport.c libpng/lodepng.c
BUILD = $(CC) $(SRCS) -I $(SDK)/include -L$(DRV) $(CFLAGS) $(LIB) -levent_core -s -o msposd

goke:
	$(eval SDK = ./sdk/gk7205v300)
	$(eval LIB = -D__GOKE__ -ldl -ldnvqe -lgk_api -lhi_mpi -lsecurec -lupvqe -lvoice_engine -ldnvqe)
	$(BUILD)

hisi:
	$(eval SDK = ./sdk/hi3516ev300)
	$(eval LIB = -D__GOKE__ -ldnvqe -lmpi -lsecurec -lupvqe -lVoiceEngine)
	$(BUILD)

star6e:
	$(eval SDK = ./sdk/infinity6)
	$(eval LIB = -D__SIGMASTAR__ -D__INFINITY6__ -D__INFINITY6E__ -lcam_os_wrapper -lm -lmi_rgn -lmi_sys -lmi_venc)
	$(BUILD)
