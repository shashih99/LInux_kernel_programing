## Old make rules ####
#obj-m = module_param.o
#
# all:
# 	make -C /lib/modules/$(shell uname -r)/build/ M=$(shell pwd) modules
# clean:
# 	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

obj-m := module_param.o

KERNELDIR ?= /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

all default:modules
install: module_install

modules modules_install help clean:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) $@

