#obj-m=hello.o


#KERNEL BUILD DIR
#KDIR=/lib/modules/$(shell uname -r)/build

#all:
#	$(MAKE) -C $(KDIR) M=$(shell pwd) modules

#clean:
#	rm -rf $(wildcard *.o *.ko *.mod.* .c* .t* test Module.symvers *.order *.markers)

obj-m = module_param.o
#obj-m =export_macro.o
#export-objs :=export_macro.o
#obj-m +=depmod.o
all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(shell pwd) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

#TARGET  := hello
#WARN    := -W -Wall -Wstrict-prototypes -Wmissing-prototypes
#INCLUDE := -isystem /lib/modules/`uname -r`/build/include
#CFLAGS  := -O2 -DMODULE -D__KERNEL__ ${WARN} ${INCLUDE}
#CC      := gcc

#${TARGET}.o: ${TARGET}.c

#.PHONY: clean

#clean:
#	rm -rf ${TARGET}.o
