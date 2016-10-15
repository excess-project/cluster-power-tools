/** @file ksample04.c
*
* @author J. Krauth
*
* Simple kernel sample with digital i/o.
*/

/** @par LICENCE
* @verbatim
 Copyright (C) 2004,2005  ADDI-DATA GmbH for the source code and the documentation.
        
        ADDI-DATA GmbH
        Dieselstrasse 3 
        D-77833 Ottersweier
        Tel: +19(0)7223/9493-0
        Fax: +49(0)7223/9493-92
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

#include <xpci3xxx.h>
#include <xpci3xxx-kapi.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ADDI-DATA");
MODULE_DESCRIPTION("XPCI-3XXX Digital io sample");
MODULE_SUPPORTED_DEVICE("XPCI-3XXX");

EXPORT_NO_SYMBOLS;

#define MODULE_NAME "ksample04"

//--------------------------------------------------------------------------------

static void xpci3xxx_run_test(struct pci_dev * pdev)
{
	int error = 0;
	unsigned long irqstate;
	uint32_t dw_DigitalInputRegister = 0;

		/* Read all digital inputs */
		xpci3xxx_lock(pdev,&irqstate);
		{
			error = i_xpci3xxx_ReadDigitalInputPort (pdev, &dw_DigitalInputRegister);
		}		
		xpci3xxx_unlock(pdev,irqstate);

		printk ("i_xpci3xxx_ReadDigitalInputPort :");
		switch (error) 
		{
			case 0: printk ("OK\n");
					printk("\nValue read : %x\n\n", dw_DigitalInputRegister);
			break;
			case 1: printk ("The handle parameter of the board is wrong\n");
			break;
		}

		printk("Digital output sample output 0, 1 and 2 are set\n");

		/*
		* Set all digital outputs. This function only
		* change the value of the digital output register
		* without to write it on the board.
		*/
		xpci3xxx_lock(pdev,&irqstate);
		{
			error = i_xpci3xxx_SetDigitalOutputRegister (pdev, 3); /* 3 = output 0, 1 and 2 are set */
		}
		xpci3xxx_unlock(pdev,irqstate);

		printk ("i_xpci3xxx_SetDigitalOutputRegister :");
		switch (error)
		{
			case 0: printk ("OK\n");
			break;
			case 1: printk ("The handle parameter of the board is wrong\n");
			break;
		}
	
		/* This function write the digital output register on the board */
		xpci3xxx_lock(pdev,&irqstate);
		{	
			error = i_xpci3xxx_SetDigitalOutputPort (pdev);
		}
		xpci3xxx_unlock(pdev,irqstate);
	
		printk ("i_xpci3xxx_SetDigitalOutputPort :");
		switch (error)
		{
			case 0: printk ("OK\n");
			break;
			case 1: printk ("The handle parameter of the board is wrong\n");
			break;
		}
}

//--------------------------------------------------------------------------------

/** Called when the driver is loaded.*/
static int __init _init_module(void)
{	
	struct pci_dev * pdev = NULL;	/* Pointer on the board structure */
	int index = 0;					/* Index of the board to use (here the fisrt board) */

		/* Get a pointer on the board to use */
		if ((pdev = xpci3xxx_lookup_board_by_index(index)) == NULL)
		{
			printk ( KERN_DEBUG "Board index %i not found\n", index);
			return 1;
		}
		printk ( KERN_DEBUG "Board index %i found\n", index);
		
		/* Test the first board found */
		xpci3xxx_run_test (pdev);
		
		printk ("Sample conclued, you can now rmmod it!\n");
		
		return 0;
}		

//--------------------------------------------------------------------------------

/** Called when the driver is unloaded.*/
static void __exit _cleanup_module(void)
{

}

//--------------------------------------------------------------------------------

module_init(_init_module);
module_exit(_cleanup_module);
