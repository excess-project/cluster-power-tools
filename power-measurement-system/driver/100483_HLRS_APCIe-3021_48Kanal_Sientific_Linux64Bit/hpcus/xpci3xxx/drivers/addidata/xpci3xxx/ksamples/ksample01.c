/** @file ksample01.c
*
* @author J. Krauth
*
* Simple kernel sample with timer interrupt.
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
MODULE_DESCRIPTION("XPCI-3XXX Timer interrupt sample");
MODULE_SUPPORTED_DEVICE("XPCI-3XXX");

EXPORT_NO_SYMBOLS;

#define MODULE_NAME "ksample01"

/* Number of iterations, one = 1 us */
#define NB_LOOP_TIMER 30000

static int interrupt_flag;

//--------------------------------------------------------------------------------

/* The user callback interrupt */
static void interrupt_callback (struct pci_dev * pdev, uint32_t dw_Dummy)
{
	static uint32_t DeviceMinorNumber;
	static uint32_t dw_InterruptSource = 0;

		/* ! Printk have to be removed in a release application ! */

		interrupt_flag = 1;
	
		/* Get the interrupt source */
		i_xpci3xxx_TestInterrupt(pdev, &DeviceMinorNumber, &dw_InterruptSource, NULL);
	
		/* Test if FIFO Empty */
		if (0xfffffffe != dw_InterruptSource)
		{
			/* Timer interrupt */
			if (dw_InterruptSource & 0x10)
				printk ("Interrupt from timer 0\n");

			/* Watchdog interrupt */
			if (dw_InterruptSource & 0x1000)
				printk ("Interrupt from watchdog\n");
		}
		else
		{
			/* Test if FIFO Full */
			if (0xffffffff != dw_InterruptSource)
			{
				printk("Driver fifo is full : %x\n", dw_InterruptSource); 
			}
		}
}

//--------------------------------------------------------------------------------

static void xpci3xxx_run_test(struct pci_dev * pdev)
{
	int error = 0;
	uint32_t dw_TimerValue = 0;
	uint32_t dw_Counter = 0;
	uint8_t b_TimerStatus = 0;
	uint8_t b_SoftwareTriggerStatus = 0;
	uint8_t b_HardwareTriggerStatus = 0;
	uint8_t b_HardwareOutputStatus = 0;
	unsigned long irqstate;
	
		printk ("Sample timer 0\n");
	
		/* Set the user interrupt callback */
		xpci3xxx_lock(pdev,&irqstate);
		{
			xpci3xxx_SetIntCallback (interrupt_callback);
			error = i_xpci3xxx_InitTimer (pdev,
											0,		// Timer number
											2,		// Timer mode
											2,		// Time unit ms
											500);	// Reload value (wait 500ms)
		}
		xpci3xxx_unlock(pdev,irqstate);

		printk ("i_xpci3xxx_InitTimer :");
		switch (error) 
		{
			case 0: printk ("OK\n");
			break;
			case 1: printk ("The handle parameter of the board is wrong\n");
			break;
			case 2: printk ("Wrong Timer number\n");
			break;
			case 3: printk ("Timer as counter already used\n");
			break;
			case 4: printk ("Timer as watchdog already used\n");
			break;
			case 5: printk ("Wrong Timer mode\n");
			break;
			case 6: printk ("Wrong Timer time unit\n");
			break;
			case 7: printk ("Wrong reload value\n");
			break;
		}

		xpci3xxx_lock(pdev,&irqstate);
		{
			error = i_xpci3xxx_EnableDisableTimerHardwareOutput (pdev,
																0,					// Timer 0
																ADDIDATA_ENABLE,	// Enable the hardware timer output
																ADDIDATA_HIGH); 	// If the timer overflow the output is set to high	
		}
		xpci3xxx_unlock(pdev,irqstate);

		printk ("i_xpci3xxx_EnableDisableTimerHardwareOutput :");
		switch (error) 
		{
			case 0: printk ("OK\n");
			break;
			case 1: printk ("The handle parameter of the board is wrong\n");
			break;
			case 2: printk ("Timer not intialised\n");
			break;
			case 3: printk ("Wrong Timer number\n");
			break;
			case 4: printk ("Timer as counter already used\n");
			break;
			case 5: printk ("Timer as watchdog already used\n");
			break;
			case 6: printk ("Wrong output flag\n");
			break;
			case 7: printk ("Wrong output level\n");
			break;
		}

		xpci3xxx_lock(pdev,&irqstate);
		{
			error = i_xpci3xxx_EnableDisableTimerInterrupt (pdev,
															0,					// Timer 0
															ADDIDATA_ENABLE);	// Interrupt enable
		}
		xpci3xxx_unlock(pdev,irqstate);

		printk ("i_xpci3xxx_EnableDisableTimerInterrupt :");
		switch (error)
		{
			case 0: printk ("OK\n");
			break;
			case 1: printk ("The handle parameter of the board is wrong\n");
			break;
			case 2: printk ("Timer not intialised\n");
			break;
			case 3: printk ("Wrong Timer number\n");
			break;
			case 4: printk ("Timer as counter already used\n");
			break;
			case 5: printk ("Timer as watchdog already used\n");
			break;
			case 6: printk ("Wrong interrupt flag\n");
			break;
			case 7: printk ("Interrupt Callback not installed\n");
			break;
		}

		xpci3xxx_lock(pdev,&irqstate);
		{
			error = i_xpci3xxx_StartTimer (pdev, 0); // Timer 0
		}
		xpci3xxx_unlock(pdev,irqstate);
		
		printk ("i_xpci3xxx_StartTimer :");
		switch (error) 
		{
			case 0: printk ("OK\n");
			break;
			case 1: printk ("The handle parameter of the board is wrong\n");
			break;
			case 2: printk ("Timer not intialised\n");
			break;
			case 3: printk ("Wrong Timer number\n");
			break;
			case 4: printk ("Timer as counter already used\n");
			break;
			case 5: printk ("Timer as watchdog already used\n");
			break;
		}
	
		/* Loop until interrupt occured */
		for (dw_Counter = 0 ; dw_Counter < NB_LOOP_TIMER ; dw_Counter ++)
		{
			xpci3xxx_lock(pdev,&irqstate);
			{
				error = i_xpci3xxx_ReadTimerValue (pdev, 0, &dw_TimerValue);
			}
			xpci3xxx_unlock(pdev,irqstate);
			
			switch (error)
			{
				case 0: printk("\r\tTimerValue : %u,", dw_TimerValue);
				break;
				case 1: printk ("The handle parameter of the board is wrong\n");
				break;
				case 2: printk ("Timer not intialised\n");
				break;
				case 3: printk ("Wrong Timer number\n");
				break;
				case 4: printk ("Timer as counter already used\n");
				break;
				case 5: printk ("Timer as watchdog already used\n");
				break;
			}

			xpci3xxx_lock(pdev,&irqstate);
			{
				error = i_xpci3xxx_ReadTimerStatus  (pdev,
													0, // Timer 0
													&b_TimerStatus,
													&b_SoftwareTriggerStatus,
													&b_HardwareTriggerStatus);
			}
			xpci3xxx_unlock(pdev,irqstate);

			switch (error)
			{
				case 0:
				break;
				case 1: printk ("i_xpci3xxx_ReadTimerStatus :");
						printk ("The handle parameter of the board is wrong\n");
				break;
				case 2: printk ("i_xpci3xxx_ReadTimerStatus :");
						printk ("Timer not intialised\n");
				break;
				case 3: printk ("i_xpci3xxx_ReadTimerStatus :");
						printk ("Wrong Timer number\n");
				break;
				case 4: printk ("i_xpci3xxx_ReadTimerStatus :");
						printk ("Timer as counter already used\n");
				break;
				case 5: printk ("i_xpci3xxx_ReadTimerStatus :");
						printk ("Timer as watchdog already used\n");
				break;
			}

			xpci3xxx_lock(pdev,&irqstate);
			{
				error = i_xpci3xxx_GetTimerHardwareOutputStatus (pdev,
																0, // Timer 0
																&b_HardwareOutputStatus);
			}
			xpci3xxx_unlock(pdev,irqstate);

			switch (error)
			{
				case 0:
				break;
				case 1: printk ("i_xpci3xxx_GetTimerHardwareOutputStatus :");
						printk ("The handle parameter of the board is wrong\n");
				break;
				case 2: printk ("i_xpci3xxx_GetTimerHardwareOutputStatus :");
						printk ("Timer not intialised\n");
				break;
				case 3: printk ("i_xpci3xxx_GetTimerHardwareOutputStatus :");
						printk ("Wrong Timer number\n");
				break;
				case 4: printk ("i_xpci3xxx_GetTimerHardwareOutputStatus :");
						printk ("Timer as counter already used\n");
				break;
				case 5: printk ("i_xpci3xxx_GetTimerHardwareOutputStatus :");
						printk ("Timer as watchdog already used\n");
				break;
			}
		
			printk("Status: %hx, Software Trigger: %hx, Hardware Trigger: %hx, Hardware Output: %hx", b_TimerStatus, b_SoftwareTriggerStatus, b_HardwareTriggerStatus, b_HardwareOutputStatus);
			
			if ((b_TimerStatus != 0) || (b_SoftwareTriggerStatus != 0) || (b_HardwareTriggerStatus != 0) || (b_HardwareOutputStatus != 0))
				printk("\n\n");

			/* If interrupt occured break this loop */
			if (interrupt_flag)
				break;

			/* Wait 1 us */
			udelay (1);
		}
	
		xpci3xxx_lock(pdev,&irqstate);
		{
			error = i_xpci3xxx_StopTimer (pdev, 0); // Timer 0
		}
		xpci3xxx_unlock(pdev,irqstate);
		
		printk ("i_xpci3xxx_StopTimer :");
		switch (error)
		{
			case 0: printk ("OK\n");
			break;
			case 1: printk ("The handle parameter of the board is wrong\n");
			break;
			case 2: printk ("Timer not intialised\n");
			break;
			case 3: printk ("Wrong Timer number\n");
			break;
			case 4: printk ("Timer as counter already used\n");
			break;
			case 5: printk ("Timer as watchdog already used\n");
			break;
		}

		xpci3xxx_lock(pdev,&irqstate);
		{
			error = i_xpci3xxx_EnableDisableTimerInterrupt (pdev,
															0, // Timer 0
															ADDIDATA_DISABLE); //Interrupt disable
		}
		xpci3xxx_unlock(pdev,irqstate);

		printk ("i_xpci3xxx_EnableDisableTimerInterrupt :");
		switch (error)
		{
			case 0: printk ("OK\n");
			break;
			case 1: printk ("The handle parameter of the board is wrong\n");
			break;
			case 2: printk ("Timer not intialised\n");
			break;
			case 3: printk ("Wrong Timer number\n");
			break;
			case 4: printk ("Timer as counter already used\n");
			break;
			case 5: printk ("Timer as watchdog already used\n");
			break;
			case 6: printk ("Wrong interrupt flag\n");
			break;
			case 7: printk ("Interrupt Callback not installed\n");
			break;
		}
	
		xpci3xxx_lock(pdev,&irqstate);
		{
			error = i_xpci3xxx_ReleaseTimer (pdev, 0); // Timer 0
		}
		xpci3xxx_unlock(pdev,irqstate);
	
		printk ("i_xpci3xxx_ReleaseTimer :");
		switch (error)
		{
			case 0: printk ("OK\n");
			break;
			case 1: printk ("The handle parameter of the board is wrong\n");
			break;
			case 2: printk ("Timer not intialised\n");
			break;
			case 3: printk ("Wrong Timer number\n");
			break;
			case 4: printk ("Timer as counter already used\n");
			break;
			case 5: printk ("Timer as watchdog already used\n");
			break;
		}
	
		/* Reset the user interrupt callback */
		xpci3xxx_ResetIntCallback ();
}

//--------------------------------------------------------------------------------

/** Called when the driver is loaded.*/
static int __init _init_module(void)
{	
	struct pci_dev * pdev = NULL;	/* Pointer on the board structure */
	int index = 0;					/* Index of the board to use (here the fisrt board) */
	
		interrupt_flag = 0;

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
