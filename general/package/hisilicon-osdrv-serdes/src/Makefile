obj-m := hi_serdes.o
hi_serdes-objs := serdes_comm.o thine_241a242_serdes.o
ccflags-y := -I $(PWD)/include

modules:
	$(MAKE) -C $(KSRC) M=$(PWD) modules

clean:
	$(MAKE) -C $(KSRC) M=$(PWD) clean
