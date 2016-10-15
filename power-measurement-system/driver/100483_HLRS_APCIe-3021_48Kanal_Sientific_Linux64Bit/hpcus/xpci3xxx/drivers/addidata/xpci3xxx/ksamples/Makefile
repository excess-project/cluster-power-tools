# the ADDIDATAINCLUDE variable may hold the path to the addidata-specific include directory
ifeq ($(ADDIDATAINC),)
# include directory has not been specified. suppose we are located inside the Linux kernel source tree
EXTRA_CFLAGS:=-I$(TOPDIR)/include/linux/addidata/ 
else
# use specified location
EXTRA_CFLAGS:=-I$(ADDIDATAINC) -D_ADDI_NO_WIN32_COMPAT -Wall -Werror
endif

# Each configuration option enables a list of files.
obj-$(CONFIG_SAMPLE01)	+= ksample01.o
obj-$(CONFIG_SAMPLE02)	+= ksample02.o
obj-$(CONFIG_SAMPLE03)	+= ksample03.o
obj-$(CONFIG_SAMPLE04)	+= ksample04.o
obj-$(CONFIG_SAMPLE05)	+= ksample05.o

# list of objects that make the module
sample01-objs := ksample01.o
sample02-objs := ksample02.o
sample03-objs := ksample03.o
sample04-objs := ksample04.o
sample05-objs := ksample05.o

# KERNEL 2.4 ------------------------------------------
ifeq ($(VERSION),2)
ifeq ($(PATCHLEVEL),4)

# The target object and module list name.
O_TARGET	:=

# Objects that export symbols.
export-objs	:= 


# The global Rules.make.
include $(TOPDIR)/Rules.make

endif
endif

