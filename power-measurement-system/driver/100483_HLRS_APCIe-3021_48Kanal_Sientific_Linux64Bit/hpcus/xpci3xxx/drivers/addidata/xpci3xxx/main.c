/** @file main.c
*
* @author Sylvain Nahas
*
* @date 05.01.06
*
* @version  $LastChangedRevision$
* @version $LastChangedDate$
*
* This module provides board related function implementation.
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
#include "vtable.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
#include <linux/sysfs.h>
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ADDI-DATA GmbH <info@addi-data.com>");
MODULE_DESCRIPTION("xpci3xxx IOCTL driver");

EXPORT_SYMBOL(xpci3xxx_get_lock);

/**@def EXPORT_NO_SYMBOLS
 * Function in this file are not exported.
 */
EXPORT_NO_SYMBOLS;


spinlock_t * xpci3xxx_get_lock(struct pci_dev *pdev)
{
        return &(XPCI3XXX_PRIVDATA(pdev)->lock);
}
//------------------------------------------------------------------------------
const char __DRIVER_NAME[] = "xpci3xxx";
//------------------------------------------------------------------------------
atomic_t xpci3xxx_count = ATOMIC_INIT(0);
unsigned int xpci3xxx_majornumber = 0;

//------------------------------------------------------------------------------
static int __devinit xpci3xxx_probe_one(struct pci_dev *dev,const struct pci_device_id *ent);
static void __devexit xpci3xxx_remove_one(struct pci_dev *dev);

//------------------------------------------------------------------------------
/** The ID table is an array of struct pci_device_id ending with a all-zero entry.
Each entry consists of:

	- vendor, device	Vendor and device ID to match (or PCI_ANY_ID)
	- subvendor,	Subsystem vendor and device ID to match (or PCI_ANY_ID)
	- subdevice class,		Device class to match. The class_mask tells which bits
	- class_mask	of the class are honored during the comparison.
	- driver_data	Data private to the driver.
*/

static struct pci_device_id xpci3xxx_pci_tbl[] __devinitdata = {
	#include "devices.ids"
	{ 0 },	/* terminate list */
};


#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
static struct class * xpci3xxx_class = NULL;
#endif

MODULE_DEVICE_TABLE (pci, xpci3xxx_pci_tbl);

//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
static struct pci_driver xpci3xxx_pci_driver = {
       .name	=	(char*)__DRIVER_NAME,
       .probe	=	xpci3xxx_probe_one,
       .remove	=	__devexit_p(xpci3xxx_remove_one),
       .id_table=	xpci3xxx_pci_tbl,
};
//------------------------------------------------------------------------------
static struct file_operations xpci3xxx_fops =
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)

        .ioctl                          = xpci3xxx_ioctl_lookup,
   #else
        .unlocked_ioctl         = xpci3xxx_ioctl_lookup,
   #endif
  .mmap		=	xpci3xxx_mmap_lookup,
  .open		=	xpci3xxx_open_lookup,
  .release	=	xpci3xxx_release_lookup,
  .fasync	= 	xpci3xxx_fasync_lookup,
};

//------------------------------------------------------------------------------
/** when module is unloaded, stop all board activities (cf interrupt)*/
static void xpci3xxx_stop_board(struct pci_dev * pdev)
{
	{
		XPCI3XXX_DEBUG_FCN();
	}
	/* reset the board in its default state */
	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_StopTimer(pdev, 0));
	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_StopTimer(pdev, 1));
	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_StopTimer(pdev, 2));

	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_StopCounter(pdev, 0));
	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_StopCounter(pdev, 1));
	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_StopCounter(pdev, 2));

	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_StopWatchdog(pdev, 0));
	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_StopWatchdog(pdev, 1));
	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_StopWatchdog(pdev, 2));

	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_EnableDisableTimerInterrupt(pdev, 0, 0));
	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_EnableDisableTimerInterrupt(pdev, 1, 0));
	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_EnableDisableTimerInterrupt(pdev, 2, 0));

	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_EnableDisableCounterInterrupt(pdev, 0, 0));
	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_EnableDisableCounterInterrupt(pdev, 1, 0));
	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_EnableDisableCounterInterrupt(pdev, 2, 0));

	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_EnableDisableWatchdogInterrupt(pdev, 0, 0));
	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_EnableDisableWatchdogInterrupt(pdev, 1, 0));
	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_EnableDisableWatchdogInterrupt(pdev, 2, 0));


	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_EnableDisableAnalogInputHardwareTrigger(pdev,0,0,0,0));
	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_EnableDisableAnalogInputSoftwareTrigger(pdev,0,0));

	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_EnableDisableAnalogInputHardwareGate(pdev,0,0));

	/* stop any sequence */
	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_StopAnalogInputSequence(pdev));
	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_ReleaseAnalogInputSequence(pdev));

	/* stop any autorefresh */
	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_StopAnalogInputAutoRefresh(pdev));
	XPCI3XXX_DEBUG_CALLRET(i_xpci3xxx_ReleaseAnalogInputAutoRefresh(pdev));


}

//-------------------------------------------------------------------
/** event: a new card is detected.
 *
 * Historically - in former ADDI-DATA drivers - data about a board has been stored
 * in a static structure.
 * This led to huge duplication of information since most of these information
 * are already present in the pci_dev struct.
 *
 * Now we manipulate this standard structure provided by the OS and we attach
 * private data using the field driver_data, if necessary.
 *
 *  */
static int __devinit xpci3xxx_probe_one(struct pci_dev *dev, const struct pci_device_id *ent)
{
	{
    	int ret = pci_enable_device(dev);
        if (ret)
        {
        	printk(KERN_ERR "%s: pci_enable_device failed\n",__DRIVER_NAME);
            return ret;
		}
	}
	pci_set_master(dev);

	/* 2.4 only : check_region is deprecated in 2.6 */
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
		/* check if BAR 0 is free */
		if (dev->device != xpci3501_BOARD_DEVICE_ID)
		{
			if ( check_mem_region(dev->resource[0].start, pci_resource_len(dev,0)) )
			{
				printk(KERN_WARNING "%s: BAR 0 (%lX) is already in use\n",__DRIVER_NAME,dev->resource[0].start);
				return -EBUSY;
			}
			/* check if BAR 1 is free */
			if ( check_region(dev->resource[1].start, pci_resource_len(dev,1)) )
			{
				printk(KERN_WARNING "%s: BAR 1 (%lX) is already in use\n",__DRIVER_NAME,dev->resource[1].start);
				return -EBUSY;
			}
			/* check if BAR 2 is free */
			if ( check_region(dev->resource[2].start, pci_resource_len(dev,2)) )
			{
				printk(KERN_WARNING "%s: BAR 2 (%lX) is already in use\n",__DRIVER_NAME,dev->resource[2].start);
				return -EBUSY;
			}
			/* check if BAR 3 is free */
			if ( check_mem_region(dev->resource[3].start, pci_resource_len(dev,3)) )
			{
				printk(KERN_WARNING "%s: BAR 3 (%lX) is already in use\n",__DRIVER_NAME,dev->resource[3].start);
				return -EBUSY;
			}
		}
		else
		{
			if ( check_region(dev->resource[0].start, pci_resource_len(dev,0)) )
			{
				printk(KERN_WARNING "%s: BAR 0 (%lX) is already in use\n",__DRIVER_NAME,dev->resource[0].start);
				return -EBUSY;
			}
			/* check if BAR 1 is free */
			if ( check_region(dev->resource[1].start, pci_resource_len(dev,1)) )
			{
				printk(KERN_WARNING "%s: BAR 1 (%lX) is already in use\n",__DRIVER_NAME,dev->resource[1].start);
				return -EBUSY;
			}
		}
	#endif // 2.4

	/* allocate a new data structure containing board private data */
	{
		struct xpci3xxx_str_BoardInformations * newboard_data = NULL;
		newboard_data = kmalloc( sizeof( struct xpci3xxx_str_BoardInformations) , GFP_ATOMIC);
		if (!newboard_data)
		{
			printk(KERN_CRIT "Can't allocate memory for new board %s\n",pci_name(dev));
			return -ENOMEM;
		}
		/* link standard data structure to the board's private one */
		//dev->driver_data = newboard_data;
		pci_set_drvdata(dev,newboard_data);

		xpci3xxx_init_priv_data(dev, newboard_data);

	}

	/* lock BAR IO ports ressources */
 	{
    	int ret = pci_request_regions(dev,__DRIVER_NAME);
        if (ret)
        {
        	printk(KERN_ERR "%s: pci_request_regions failed\n",__DRIVER_NAME);
            /* free all allocated ressources here*/
            kfree(XPCI3XXX_PRIVDATA(dev));
            return ret;
		}
    }


	/* Map the memory Address 0*/
	XPCI3XXX_PRIVDATA(dev)->dw_MemBaseAddress0 =  ioremap (dev->resource[0].start,pci_resource_len(dev,0));

	/* Map the memory Address 3*/
	XPCI3XXX_PRIVDATA(dev)->dw_MemBaseAddress3 =  ioremap (dev->resource[3].start,pci_resource_len(dev,3));


	/* register interrupt */
	if ( xpci3xxx_register_interrupt(dev) )
	{
		/* failed, clean previously allocated resources */
		kfree(XPCI3XXX_PRIVDATA(dev));

	    /* Unmap the memory Address 0*/
	    iounmap(GET_MEM_BAR0(dev) );

	    /* Unmap the memory Address 3*/
	    iounmap(GET_MEM_BAR3(dev) );

		pci_release_regions(dev);
	}


	{
		/* increase the global board count */
		atomic_inc(&xpci3xxx_count);
		printk(KERN_INFO "%s: board %s managed (minor number will be %d) \n",__DRIVER_NAME, pci_name(dev), atomic_read(&xpci3xxx_count)-1);
	}

	/* create /proc entry */
	xpci3xxx_proc_create_device(dev, atomic_read(&xpci3xxx_count)-1);

    /* Save the minor number */
    XPCI3XXX_PRIVDATA(dev)->i_MinorNumber = atomic_read(&xpci3xxx_count)-1;

    /* add to the list of known device */
    xpci3xxx_known_dev_append(dev);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
    while(1)
	{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
		struct class_device *cdev;
#else
		struct device *cdev;
#endif // 2.6.26

		int minor = (atomic_read(&xpci3xxx_count)-1);

        /* don't execute if class not exists */
        if (IS_ERR(xpci3xxx_class))
                break;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
		cdev = class_device_create (xpci3xxx_class, NULL, MKDEV(xpci3xxx_majornumber, minor), NULL, "%s_%d", __DRIVER_NAME, minor);
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)
		cdev = device_create(xpci3xxx_class, NULL, MKDEV(xpci3xxx_majornumber, minor), "%s_%d", __DRIVER_NAME, minor);
#else
		cdev = device_create(xpci3xxx_class, NULL, MKDEV(xpci3xxx_majornumber, minor), NULL, "%s_%d", __DRIVER_NAME, minor);
#endif // 2.6.26

		if (IS_ERR(cdev))
		{
			printk (KERN_WARNING "%s: class_device_create error\n", __DRIVER_NAME);
		}
		break;
	}
#endif

	return 0;
}


//------------------------------------------------------------------------------
/** This function registers the driver with register_chrdev.
 *
 * @todo For kernel 2.6, use udev
 *  */

static int xpci3xxx_register(void)
{

	//Registration of driver
	xpci3xxx_majornumber = register_chrdev(0, __DRIVER_NAME, &xpci3xxx_fops);

	if (xpci3xxx_majornumber < 0)
	{
		printk (KERN_ERR "%s: register_chrdev returned %d ... aborting\n",__DRIVER_NAME,xpci3xxx_majornumber);
		return -ENODEV;
	}
	return 0;
}

//------------------------------------------------------------------------------
/** Called when module loads. */
static int __init xpci3xxx_init(void)
{

	xpci3xxx_init_vtable(xpci3xxx_vtable);

	if( xpci3xxx_register() )
	{
		return -ENODEV;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	xpci3xxx_class = class_create (THIS_MODULE, __DRIVER_NAME);
	if (IS_ERR(xpci3xxx_class))
 	{
 		printk (KERN_WARNING "%s: class_create error\n",__DRIVER_NAME );
 	}
#endif


	/* registred, now create root /proc entry */
	xpci3xxx_proc_init();

	printk(KERN_INFO "%s: loaded\n",__DRIVER_NAME);

	/* now, subscribe to PCI bus subsystem  */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
	/** @note The module will load anyway even if the call to pci_module_init() fails */
	if ( pci_module_init (&xpci3xxx_pci_driver) != 0 )
#else
	if ( pci_register_driver (&xpci3xxx_pci_driver) != 0)
#endif
	{
		printk("%s: no device found\n",__DRIVER_NAME);
		xpci3xxx_pci_driver.name = "";
	}

	return 0;
}

//-------------------------------------------------------------------
/** event: a card is removed (also called when module is unloaded) */
static void __devexit xpci3xxx_remove_one(struct pci_dev *dev)
{

	/* stop board activities */
	xpci3xxx_stop_board(dev);

	/* register interrupt */
	xpci3xxx_deregister_interrupt(dev);

	/* do OS-dependant thing we don't really want to know of :) */
	pci_disable_device(dev);

    /* Unmap the memory Address 0*/
    iounmap(GET_MEM_BAR0(dev));

    /* Unmap the memory Address 3*/
    iounmap(GET_MEM_BAR3(dev));

	/* deallocate BAR IO Ports ressources */
	pci_release_regions(dev);

	/* free private device data (uncomment) */
	kfree(XPCI3XXX_PRIVDATA(dev));

	/* delete associated /proc entry */
	xpci3xxx_proc_release_device(dev);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	while(1)
	{
		int minor = (atomic_read(&xpci3xxx_count)-1);

		/* don't execute if class not exists */
		if(IS_ERR(xpci3xxx_class))
				break;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
		class_device_destroy (xpci3xxx_class, MKDEV(xpci3xxx_majornumber, minor));
#else
		device_destroy (xpci3xxx_class, MKDEV(xpci3xxx_majornumber, minor));
#endif

		break;
	}
#endif

	/* decrement global board count*/
	atomic_dec(&xpci3xxx_count);

    /* remove from the list of known devices */
    xpci3xxx_known_dev_remove(dev);

	printk("%s: board %s unloaded \n",__DRIVER_NAME,pci_name(dev));

}

//------------------------------------------------------------------------------
/** This function unregisters the driver with unregister_chrdev.
 *
 * @todo For kernel 2.6, use udev
 *  */

static void xpci3xxx_unregister(void)
{
	unregister_chrdev(xpci3xxx_majornumber, __DRIVER_NAME);
}

//-------------------------------------------------------------------
/** Called when module is unloaded. */
static void __exit xpci3xxx_exit(void)
{

	/* unsubscribe to PCI bus subsystem */
	if (xpci3xxx_pci_driver.name[0])
	{
		pci_unregister_driver (&xpci3xxx_pci_driver);
	}

	/* unsubscribe to /dev VFS */
	xpci3xxx_unregister();

	/* delete /proc root */
	xpci3xxx_proc_release();

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
	class_destroy (xpci3xxx_class);
#endif

}
//------------------------------------------------------------------------------
module_exit(xpci3xxx_exit);
module_init(xpci3xxx_init);
