EXTRA_CFLAGS += -I $(PWD)/include -I $(KSRC)/drivers/sstar/include
EXTRA_CFLAGS += -D SENSOR_MODULE_VERSION=$(OPENIPC_SOC_MODEL)-$(OPENIPC_SOC_FAMILY)
obj-m := $(patsubst $(PWD)/%.c, %.o, $(wildcard $(PWD)/sensor/*.c))

modules:
	$(MAKE) -C $(KSRC) M=$(PWD) modules

clean:
	$(MAKE) -C $(KSRC) M=$(PWD) clean
