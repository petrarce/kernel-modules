ifneq ($(KERNELRELEASE),)

#main object file, which will be used, to create target .ko file. 
#(e.g. if obj-m = my.o -> my.ko)
obj-m = $(MAIN_OBJ)
#other objects, that will be linked into kernel module
module-objs = $(SECONDARY_OBJ)
else

ifeq ($(KPATH),)

KPATH=/lib/modules/`uname -r`/build
endif

.PHONY = default 

default:
	#make command to compile external kernel module
	#-C - path to kernel build environment, 
	#M=... - specifies absolute path to external module resources
	make -C ${KPATH} M=${PWD}

endif
