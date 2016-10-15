
PROJPATH:=${CURDIR}
KERNELSRCDIR:=/usr/src/kernels/2.6.32-431.17.1.el6.BPA.x86_64/
#KERNELSRCDIR:=/usr/src/kernels/2.6.32-431.11.2.el6.BPA.x86_64/
ADDIDATAINC:=$(PROJPATH)/xpci3xxx/include/linux/addidata

all:
	$(MAKE) -C hpc KERNELSRCDIR=$(KERNELSRCDIR) ADDIDATAINC=$(ADDIDATAINC) || exit 1
	$(MAKE) -C $(PROJPATH)/xpci3xxx/drivers/addidata/xpci3xxx -f _makefile 2.6 EXTRA_FLAGS=$(EXTRA_FLAGS) KERNELSRCDIR=$(KERNELSRCDIR) ADDIDATAINC=$(ADDIDATAINC) || exit 1
			
clean:
	$(MAKE) -C hpc clean || exit 1
	$(MAKE) -C $(PROJPATH)/xpci3xxx/drivers/addidata/xpci3xxx -f _makefile clean || exit 1
		
install:
	$(MAKE) -C $(PROJPATH)/xpci3xxx/drivers/addidata/xpci3xxx -f _makefile install_26 KERNELSRCDIR=$(KERNELSRCDIR) ADDIDATAINC=$(ADDIDATAINC) || exit 1
