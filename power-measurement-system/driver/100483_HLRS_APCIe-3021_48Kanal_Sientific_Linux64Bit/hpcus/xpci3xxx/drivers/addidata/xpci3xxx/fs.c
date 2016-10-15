/** @file fs.c
*
* @author Sylvain Nahas
*
* @date 05.01.06
* 
* @version  $LastChangedRevision$
* @version $LastChangedDate$
* 
* This module provides FS related function implementation.
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

#include "xpci3xxx-private.h"

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
        #include <linux/config.h>
#else
        #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)
                #include <linux/autoconf.h>
        #else
                #include <generated/autoconf.h>
        #endif
#endif

/**@def EXPORT_NO_SYMBOLS
 * Function in this file are not exported.
 */
EXPORT_NO_SYMBOLS;

//------------------------------------------------------------------------------
/** Used for asynchronous notification.
* 
*
*/
int
xpci3xxx_fasync_lookup(int fd, struct file *filp, int mode)
{
	return fasync_helper(fd, filp, mode, & (XPCI3XXX_PRIVDATA(filp->private_data)->async_queue) );
}

//------------------------------------------------------------------------------
/** find a xpci3xxx device of a given index in the system PCI device list.
 * @param index The index (!!minor number!!) to lookup
 * @return A pointer to the device or NULL
 *
 * This function is used to map a minor number to an actual device.
 */
//static __inline__  struct pci_dev * xpci3xxx_lookup_board_by_index(unsigned int index)
//{
//	struct pci_dev * dev = NULL;
//	int i = 0;	
//		
//	// xpci-3003
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3003_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;	
//	}      
//		
//	// xpci-3002
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3002_16_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//		
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3002_8_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3002_4_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//
//	// xpci-3116
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3116_16_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//		
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3116_8_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//
//	// xpci-3110
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3110_16_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3110_8_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//
//	// xpci-3106
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3106_16_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3106_8_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//	
//	// xpci-3100
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3100_16_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3100_8_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//	
//	// xpci-3016
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3016_16_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//		
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3016_8_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3016_4_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//
//	// xpci-3010
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3010_16_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3010_8_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3010_4_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//
//	// xpci-3006
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3006_16_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3006_8_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//	
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3006_4_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//	
//	// xpci-3000
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3000_16_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3000_8_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//	
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3000_4_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//	
//	// xpci-3500
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3500_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	}      
//
//     // xpci-3002 Audi
//     while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3002_16_AUDI_BOARD_DEVICE_ID,dev)) )
//     {
//          if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//               return dev;
//     }      
//
//	// xpci-3501
//	while ( (dev = pci_find_device(xpci3xxx_BOARD_VENDOR_ID,xpci3501_BOARD_DEVICE_ID,dev)) )
//	{
//		if (XPCI3XXX_PRIVDATA(dev)->i_MinorNumber == index)
//			return dev;
//	} 
//	
//	return NULL;
//
//}
//------------------------------------------------------------------------------
/** open() function of the module for the xpci3xxx, with lookup though global OS PCI list 
* this is for files unmanaged by the driver itself and use the minor device number to identify the board.
*
* When opening, the pci_dev associated to the minor number is looked up
* and associated with the file structure. It avoid further lookup when calling ioctl()
* 
*
*/
int xpci3xxx_open_lookup (struct inode *inode, struct file *filp)
{
   if ( xpci3xxx_INDEX_NOT_VALID(&xpci3xxx_count, MINOR(inode->i_rdev) ) )
   {
   	return -ENODEV;
   }
      
   filp->private_data = xpci3xxx_lookup_board_by_index(MINOR(inode->i_rdev) );  
   
   MOD_INC_USE_COUNT;
   return 0;   
}
//------------------------------------------------------------------------------
/** release() function of the module for the xpci3xxx, with lookup though global OS PCI list 
* this is for files unmanaged by the driver itself and use the minor device number to identify the board
* 
*
*/
int xpci3xxx_release_lookup (struct inode *inode,struct file *filp)
{

   if ( xpci3xxx_INDEX_NOT_VALID(&xpci3xxx_count, MINOR(inode->i_rdev) ) )
   {
   	return -ENODEV;
   }

	if ( (filp->f_flags) & FASYNC)
	{
		xpci3xxx_fasync_lookup(-1, filp, 0);
	}
	

   MOD_DEC_USE_COUNT;
   return 0;
}
//------------------------------------------------------------------------------
/** ioctl() function of the module for the xpci3xxx, with lookup though global OS PCI list 
* this is for files unmanaged by the driver itself and use the minor device number to identify the board
* 
*
*/
//------------------------------------------------------------------------------
/** ioctl() function of the module for the APCI-3200, with lookup though global OS PCI list
* this is for files unmanaged by the driver itself and use the minor device number to identify the board
*/
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
        int xpci3xxx_ioctl_lookup (struct inode *inode,
                                   struct file *filp,
                                   unsigned int cmd,
                                   unsigned long arg)
#else
        long xpci3xxx_ioctl_lookup (struct file *filp,
                                   unsigned int cmd,
                                   unsigned long arg)
#endif
        {
                int ret = 0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36)
                struct inode *inode = filp->f_dentry->d_inode;
#endif

   if ( xpci3xxx_INDEX_NOT_VALID(&xpci3xxx_count, MINOR(inode->i_rdev) ) )
   {
        return -ENODEV;
   }  

   if (!filp->private_data) // private data is initialised to NULL
   {
        printk(KERN_CRIT "%s: %s: board data should be affected but is not (did you call open() before ioctl() ?) \n",__DRIVER_NAME, __FUNCTION__);
        return -EBADFD;
   }

//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36) && LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39)
//   lock_kernel();
//#endif

   ret = xpci3xxx_do_ioctl( (struct pci_dev*) filp->private_data, cmd, arg);

//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36) && LINUX_VERSION_CODE < KERNEL_VERSION(2,6,39)
//   unlock_kernel();
//#endif

   return ret;
}


/** Set the shared memory for the board.
* 
*
 */
int xpci3xxx_mmap_lookup (struct file *filp, struct vm_area_struct *vma)
{

	int ret;

// 	//BEGIN BF151
// 	printk("Bigphys address=%X\nvirt_to_phys()=%X\n\n",XPCI3XXX_PRIVDATA((struct pci_dev*) filp->private_data)->kmalloc_area,virt_to_phys(XPCI3XXX_PRIVDATA((struct pci_dev*) filp->private_data)->kmalloc_area));
// 	//END
	/* 2.4 only : remap_page_range is deprecated in 2.6 */
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
		ret= remap_page_range (vma->vm_start, virt_to_phys(XPCI3XXX_PRIVDATA((struct pci_dev*) filp->private_data)->kmalloc_area), vma->vm_end-vma->vm_start, PAGE_SHARED);     
	#else
		ret = remap_pfn_range (vma, vma->vm_start, virt_to_phys(XPCI3XXX_PRIVDATA((struct pci_dev*) filp->private_data)->kmalloc_area) >> PAGE_SHIFT, vma->vm_end-vma->vm_start, PAGE_SHARED);     
	#endif // 2.4

	if (ret !=0)
    	return -EAGAIN;
    	
	return (0);
}




