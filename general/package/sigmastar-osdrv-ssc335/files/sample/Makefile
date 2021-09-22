
CC := ../../../../../output/host/bin/arm-openipc-linux-uclibcgnueabihf-gcc

LIBS_PATH = ../lib
INCLUDE_PATH = ../include

CFLAGS := -I$(INCLUDE_PATH)
LINK_FLAGS := -L$(LIBS_PATH)
LIBS := -lmi_sys  -lmi_sensor  -lmi_vif  -lmi_vpe  -lmi_isp  -lcus3a  -lispalgo  -lmi_venc  -Wl,-Bstatic -Wl,--whole-archive -lcam_os_wrapper -Wl,--no-whole-archive -Wl,-Bdynamic
#LIBS := -lmi_sys  -lmi_sensor  -lmi_vif  -lmi_vpe  -lmi_isp  -lcus3a  -lispalgo  -lmi_venc  -lcam_os_wrapper

all: test_venc

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test_venc: test_venc.o
	$(CC) test_venc.o -o $@ $(LINK_FLAGS) $(LIBS)
	make clean

clean:
	-rm -f *.o
	#-rm -f test_venc

.PHONY: all clean