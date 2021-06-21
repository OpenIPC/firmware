
TOP=$(shell pwd)

GNU_TARGET := arm-buildroot-linux-uclibcgnueabihf
#GNU_TARGET := arm-openipc-linux-uclibcgnueabihf
CC := $(GNU_TARGET)-gcc

#LIBS_PATH = sstar-libs/ipc/i6b0/common/uclibc/4.9.4/
#INCLUDE_PATH = sstar-libs/include

LIBS_PATH = $(TOP)/sstar-libs
INCLUDE_PATH = $(TOP)/sstar-libs/include

CFLAGS := -I$(INCLUDE_PATH)
LINK_FLAGS := -L$(LIBS_PATH)/mi_libs/dynamic -L$(LIBS_PATH)/ex_libs/static
LIBS := -lmi_sys -lmi_sensor -lmi_vif -lmi_vpe -lmi_isp -lcus3a -lispalgo -lmi_venc -Wl,-Bstatic -Wl,--whole-archive -lcam_os_wrapper -Wl,--no-whole-archive -Wl,-Bdynamic

all: test_venc_ssc335

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test_venc_ssc335: test_venc_ssc335.o
	$(CC) test_venc_ssc335.o -o $@ $(LINK_FLAGS) $(LIBS)

clean:
	-rm -f *.o
	-rm -f test_venc_ssc335

.PHONY: all clean