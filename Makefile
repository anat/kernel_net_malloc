TARGET		=	netmalloc

KDIR		=	/lib/modules/$(shell uname -r)/build
PWD		=	$(shell pwd)

TARGET		= 	netmalloc

obj-m		:=	netmalloc.o
netmalloc-objs	:=	main.o mem.o

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	rm -f *~
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

load:
	sudo insmod $(TARGET).ko

unload:
	sudo rmmod $(TARGET).ko
