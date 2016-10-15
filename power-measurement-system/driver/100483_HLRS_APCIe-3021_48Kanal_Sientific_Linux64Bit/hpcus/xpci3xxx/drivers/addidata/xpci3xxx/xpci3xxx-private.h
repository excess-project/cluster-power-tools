/** @file xpci3xxx-private.h
*
* @author Sylvain Nahas
*
* @date 05.01.06
*
* @version  $LastChangedRevision$
* @version $LastChangedDate$
*
* This header contains private definitions (not to be seen by external code)
*
*/

/** @par LICENCE
* @verbatim
Copyright (C) 2004,2005  ADDI-DATA GmbH for the source code and the documentation.

ADDI-DATA GmbH
Airpark Business Center
Airport Boulevard B210
77836 Rheinmünster
Germany
Tel: +49(0)7229/1847-0
http://www.addi-data-com
info@addi-data.com

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You shoud find the complete GPL in the COPYING file accompanying
this source code.
* @endverbatim
*/

#ifndef __xpci3xxx_PRIVATE__
#define __xpci3xxx_PRIVATE__

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
    #include <linux/config.h>
#else
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)
		#include <linux/autoconf.h>
	#else
		#include <generated/autoconf.h>
	#endif

	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36) && LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39)
		#include <linux/smp_lock.h>
	#endif
#endif

#include <linux/spinlock.h>
#include <linux/ioctl.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/pci.h> // struct pci_dev
#include <asm/io.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0)
	#include <asm/system.h>
#endif
#include <asm/uaccess.h>
#include <asm/bitops.h>
#include <linux/sched.h>

#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/version.h>

/* Shared memory */
#include <linux/mman.h>
#include <linux/vmalloc.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)	/* 2.4  */
#include <linux/wrapper.h>
#endif // 2.4
#include <linux/signal.h>

#include <apci.h>
#include <xpci3xxx.h>
#include <xpci3xxx-kapi.h>
#include <eeprom.h>
#include "eeprom-private-kapi.h"

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,27)
	#ifndef __user
		#define __user
	#endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
	#ifndef __iomem
		#define __iomem
	#endif
#endif


/** ressources of a board */

static __inline__ unsigned long GET_BAR0(struct pci_dev * dev)
{
	return dev->resource[0].start;
}

static __inline__ unsigned long GET_BAR1(struct pci_dev * dev)
{
	return dev->resource[1].start;
}

static __inline__ unsigned long GET_BAR2(struct pci_dev * dev)
{
	return dev->resource[2].start;
}

static __inline__ unsigned long GET_BAR3(struct pci_dev * dev)
{
	return dev->resource[3].start;
}


static __inline void INPDW (uint32_t dw_Address, uint32_t* pdw_LongValue)
{
	uint32_t dw_ReadValue;

	dw_ReadValue = inl (dw_Address);

	*(pdw_LongValue) = dw_ReadValue;

}

static __inline void OUTPDW (uint32_t dw_Address,uint32_t dw_LongValue)
{
	outl (dw_LongValue,dw_Address);

}

static __inline void INPDW_MEM (void __iomem * dw_Address, uint32_t dw_Offset, uint32_t* pdw_LongValue)
{
	*(pdw_LongValue) = readl(dw_Address + dw_Offset);
}

static __inline void OUTPDW_MEM (void __iomem * dw_Address,uint32_t dw_Offset, uint32_t dw_LongValue)
{

	writel(dw_LongValue,dw_Address + dw_Offset);

}


/** Utility fonction that returns 0 if the given index corresponds to a card already configured.
*
* @param count Number of board
* @param index Minor number to check
*
* @return 0 if minor number valid, not 0 otherwise
*
* count ranges from 1 to ...
* index ranges from 0 to ...
*
* */

static __inline__ int xpci3xxx_INDEX_NOT_VALID(atomic_t * count , unsigned int index )
{
	volatile unsigned int __count =  atomic_read(count);
	if (index >= __count) return 1;
	return 0;

}


/* number of board detected by the kernel */
extern atomic_t xpci3xxx_count;

extern const char __DRIVER_NAME[];

/* major number (attributed by the OS) */
extern unsigned int xpci3xxx_majornumber;

extern void (*xpci3xxx_private_InterruptCallback) (struct pci_dev * pdev, uint32_t dw_InterruptSource);

/* /dev function */
int xpci3xxx_fasync_lookup(int fd, struct file *filp, int mode);
int xpci3xxx_open_lookup (struct inode *inode, struct file *filp);
int xpci3xxx_release_lookup (struct inode *inode,struct file *filp);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
        int xpci3xxx_ioctl_lookup (struct inode *inode,
                                                           struct file *filp,
                                                           unsigned int cmd,
                                                           unsigned long arg);
#else
        long xpci3xxx_ioctl_lookup (struct file *filp,
                                                                unsigned int cmd,
                                                                unsigned long arg);
#endif
int xpci3xxx_mmap_lookup (struct file *filp, struct vm_area_struct *vma);

/* kernel-side accessible API */
//int xpci3xxx_do_ioctl(struct pci_dev * pdev, unsigned int cmd, unsigned long arg);

/*/proc functions  */
void xpci3xxx_proc_init(void);
void xpci3xxx_proc_release(void);
void xpci3xxx_proc_create_device(struct pci_dev * dev, unsigned int minor_number);
void xpci3xxx_proc_release_device(struct pci_dev * dev);

/* interrupt related function */
int xpci3xxx_register_interrupt(struct pci_dev * pdev);
int xpci3xxx_deregister_interrupt(struct pci_dev * pdev);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,27)
#undef __user
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#undef __iomem
#endif

#include "api.h"
#include "privdata.h"

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,27)
#define __user
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#define __iomem
#endif

static __inline__ void __iomem * GET_MEM_BAR0(struct pci_dev * dev)
{

	return(XPCI3XXX_PRIVDATA(dev)->dw_MemBaseAddress0);
}

static __inline__ void __iomem * GET_MEM_BAR3(struct pci_dev * dev)
{

	return(XPCI3XXX_PRIVDATA(dev)->dw_MemBaseAddress3);
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,27)
#undef __user
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#undef __iomem
#endif

#endif //__xpci3xxx_PRIVATE__
