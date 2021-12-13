obj-m =character_driver_skel.o 
obj-m += character_driver_udev.o
#TARGET=application

#CC_COMPILER=gcc

#${TARGET}:
#	${CC_COMPILER} $@.c -o $@

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(shell pwd) modules 
#build: ${TARGET}

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
#clear:
#	rm -f ${TARGET}
