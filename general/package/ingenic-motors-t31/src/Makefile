#CROSS_COMPILE ?= mips-linux-gnu-
#KDIR := ${ISVP_ENV_KERNEL_DIR}

############################################################################

PWD:=$(shell pwd)
INSTALL_DIR := $(PWD)/output

DEFS =

MOTORS_NAME := sample_motor

$(MOTORS_NAME)-objs := motor.o
obj-m := $(MOTORS_NAME).o


EXTRA_CFLAGS := $(DEFS)

all: modules install

modules: clean
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(shell pwd) modules -j8

strip:
	$(CROSS_COMPILE)strip $(INSTALL_DIR)/*.ko --strip-unneeded

install: modules
	mkdir -p $(INSTALL_DIR)
	chmod 777 $(INSTALL_DIR)
	cp *.ko $(INSTALL_DIR)

clean:
	@rm -rf *.o *~ .depend .*.cmd  *.mod.c .tmp_versions *.ko *.symvers modules.order
	make -C $(KDIR) M=$(PWD) ARCH=$(ARCH) clean

.PHONY: modules clean
