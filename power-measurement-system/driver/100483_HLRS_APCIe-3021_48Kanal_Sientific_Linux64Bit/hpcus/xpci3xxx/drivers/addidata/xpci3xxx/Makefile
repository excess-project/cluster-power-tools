# the ADDIDATAINCLUDE variable may hold the path to the addidata-specific include directory
ifeq ($(ADDIDATAINC),)
# include directory has not been specified. suppose we are located inside the Linux kernel source tree
EXTRA_CFLAGS:=-I$(TOPDIR)/include/linux/addidata/ 
else
# use specified location
EXTRA_CFLAGS:=-I$(ADDIDATAINC) -D_ADDI_NO_WIN32_COMPAT
endif

#EXTRA_CFLAGS+=-DCONFIG_XPCI3XXX_DEBUG

# Each configuration option enables a list of files.
obj-$(CONFIG_XPCI3XXX_IOCTL) += xpci3xxx.o

# list of objects that make the module
xpci3xxx-objs := main.o fs.o procfs.o ioctl.o interrupt.o vtable.o timer.o\
					counter.o watchdog.o k_timer.o k_counter.o k_watchdog.o\
					primary_tcw.o digital_input.o digital_output.o ttl.o\
					analog_input.o analog_output.o primary_di.o primary_do.o\
					primary_ttl.o primary_ai.o primary_ao.o shared_memory.o\
					primary_dma.o eeprom-kapi.o knowndev.o io_access.o


# KERNEL 2.4 ------------------------------------------
ifeq ($(VERSION),2)
ifeq ($(PATCHLEVEL),4)

# The target object and module list name.
O_TARGET	:= driver.o

# Objects that export symbols.
export-objs	:= main.o k_timer.o k_counter.o k_watchdog.o primary_di.o\
				primary_do.o primary_ttl.o primary_ai.o primary_ao.o\
				shared_memory.o primary_dma.o fs.o interrupt.o knowndev.o

# The global Rules.make.
include $(TOPDIR)/Rules.make

# Link rules
xpci3xxx.o: $(xpci3xxx-objs)
	$(LD) -r -o $@ $(xpci3xxx-objs)
	
endif
endif
