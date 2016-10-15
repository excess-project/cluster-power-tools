/** @file ksample03.c
*
* @author J. Krauth
*
* Simple kernel sample with analog input auto refresh.
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
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/kthread.h>

#include <xpci3xxx.h>
#include <xpci3xxx-kapi.h>



MODULE_LICENSE("GPL");
MODULE_AUTHOR("ADDI-DATA");
MODULE_DESCRIPTION("XPCI-3XXX Analog input auto refresh");
MODULE_SUPPORTED_DEVICE("XPCI-3XXX");

EXPORT_NO_SYMBOLS;

/** The name of the module. */
#define MODULE_NAME "ksample03"

#define NUMBER_OF_BOARDS	1

#define NUMBER_OF_CHANNELS					16
#define NUMBER_OF_CYCLE						10

/* Global pointer on the board structure. Global because it is used in module init and cleanup */
static struct pci_dev * _pdev[NUMBER_OF_BOARDS];

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

static struct task_struct *thread_handle;

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

static int thread (void *arg)
{
	int error = 0;
	unsigned long irqstate = 0;
	uint32_t val[ADDIDATA_MAX_AI] = {0};
	uint32_t cycleIndex = 0;
	uint32_t prevCycleIndex = 1;
	struct pci_dev *pdev = (struct pci_dev *) arg;
	int ch = 0;

	printk ("Cycle     \t");
	for (ch=0; ch<NUMBER_OF_CHANNELS; ch++)
		printk ("CH%i\t", ch);

	while (!kthread_should_stop())
	{
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(HZ/4);

		xpci3xxx_lock(pdev,&irqstate);
		{
			error = i_xpci3xxx_ReadAnalogInputAutoRefreshValueAndCounter (pdev, val, &cycleIndex);
		}
		xpci3xxx_unlock(pdev,irqstate);

		switch (error)
		{
			case 0:
			{
				/* If new value are available */
				if (cycleIndex != prevCycleIndex)
				{
					printk ("\n%10u\t", cycleIndex);

					for (ch=0; ch<NUMBER_OF_CHANNELS; ch++)
						printk ("%x\t", val[ch]);
				}
				prevCycleIndex = cycleIndex;
			}
			break;

			case 1: printk ("The handle parameter of the board is wrong\n");
			return 0;
			break;

			case 2: printk ("Auto refresh not initialized\n");
			return 0;
			break;
		}
	}

	return 0;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

static int ksample02_init_and_read(struct pci_dev * pdev)
{
	int error = 0;
	unsigned long irqstate;


    uint8_t  pb_ChannelList[NUMBER_OF_CHANNELS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    uint8_t  pb_Gain[NUMBER_OF_CHANNELS] = {ADDIDATA_1_GAIN};
    uint8_t  pb_Polarity[NUMBER_OF_CHANNELS] = {ADDIDATA_BIPOLAR};

		printk ("Analog input sequence (DMA and interrupt)\n");

		xpci3xxx_lock(pdev,&irqstate);
		{
			error = i_xpci3xxx_InitAnalogInputAutoRefresh (pdev,
															NUMBER_OF_CHANNELS,							// Number of channels
															pb_ChannelList,
															pb_Gain,
															pb_Polarity,
															ADDIDATA_DISABLE,							// Single Diff
															0,											// Number of sequence (0: Continuous mode)
															ADDIDATA_DISABLE,							// Delay mode
															0,											// Delay time unit
															0);											// Delay time
		}
		xpci3xxx_unlock(pdev,irqstate);

		printk ("i_xpci3xxx_InitAnalogInputAutoRefresh :");
		switch (error)
		{
			case 0: printk ("OK\n");
			break;
			case 1: printk ("The handle parameter of the board is wrong\n");
			break;
			case 2: printk ("Wrong number of channels\n");
			break;
			case 3: printk ("Wrong channel number\n");
			break;
			case 4: printk ("Wrong gain factor\n");
			break;
			case 5: printk ("Wrong polarity\n");
			break;
			case 6: printk ("Wrong single diff parameter\n");
			break;
			case 7: printk ("Wrong delay mode parameter\n");
			break;
			case 8: printk ("Wrong delay time unit parameter\n");
			break;
			case 9: printk ("Wrong delay time parameter\n");
			break;
			case 10: printk ("A conversion is already started\n");
			break;
		}

		if (error)
			return -1;

		xpci3xxx_lock(pdev,&irqstate);
		{
			error = i_xpci3xxx_StartAnalogInputAutoRefresh (pdev,
															ADDIDATA_MICRO_SECOND,	// Convert time unit
															5);					// Convert time
		}
		xpci3xxx_unlock(pdev,irqstate);

		printk ("i_xpci3xxx_StartAnalogInputAutoRefresh :");
		switch (error)
		{
			case 0: printk ("OK\n");
			break;
			case 1: printk ("The handle parameter of the board is wrong\n");
			break;
			case 2: printk ("Auto refresh not initialised\n");
			break;
			case 3: printk ("Wrong convert time unit\n");
			break;
			case 4: printk ("Wrong convert time\n");
			break;
			case 5: printk ("A conversion is already started\n");
			break;
		}

		/* Initialize and start a kernel thread to get autorefresh values */
		thread_handle = kthread_run (thread, pdev, "acquisition_thread");
		if (IS_ERR (thread_handle))
			return -1;

	return error;
}

//--------------------------------------------------------------------------------

static void ksample02_release(struct pci_dev * pdev)
{
	int error = 0;
	unsigned long irqstate;

		xpci3xxx_lock(pdev,&irqstate);
		{
			error = i_xpci3xxx_StopAnalogInputAutoRefresh (pdev);
		}
		xpci3xxx_unlock(pdev,irqstate);

		printk ("i_xpci3xxx_StopAnalogInputAutoRefresh :");
		switch (error)
		{
			case 0: printk ("OK\n");
			break;
			case 1: printk ("The handle parameter of the board is wrong\n");
			break;
			case 2: printk ("Auto refresh not initialised\n");
			break;
		}

		xpci3xxx_lock(pdev,&irqstate);
		{
			error = i_xpci3xxx_ReleaseAnalogInputAutoRefresh (pdev);
		}
		xpci3xxx_unlock(pdev,irqstate);

		printk ("i_xpci3xxx_ReleaseAnalogInputAutoRefresh :");
		switch (error)
		{
			case 0: printk ("OK\n");
			break;
			case 1: printk ("The handle parameter of the board is wrong\n");
			break;
			case 2: printk ("Auto refresh not initialised\n");
			break;
			case 3: printk ("A conversion is already started\n");
			break;
		}
}

//--------------------------------------------------------------------------------

/** Called when the driver is loaded.*/
static int __init _init_module(void)
{
	int index = 0; /* Index of the board to use (here the fisrt board) */

		/* Get a pointer on the board to use */
		for (index=0; index < NUMBER_OF_BOARDS; index++)
		{
			if ((_pdev[index] = xpci3xxx_lookup_board_by_index(index)) == NULL)
			{
				printk ( KERN_DEBUG "Board index %i not found\n", index);
				return -ENODEV;
			}
			printk ( KERN_DEBUG "Board index %i found (slot: %s)\n", index,pci_name(_pdev[index]));
		}

		/* Test the first board found */
		for (index=0; index<NUMBER_OF_BOARDS; index++)
		{
			if ( ksample02_init_and_read (_pdev[index]) )
			{
				return -EIO;
			}
		}

		return 0;
}

//--------------------------------------------------------------------------------

/** Called when the driver is unloaded.*/
static void __exit _cleanup_module(void)
{
	int index = 0;

	kthread_stop (thread_handle);

	for (index=0; index<NUMBER_OF_BOARDS; index++)
		ksample02_release (_pdev[index]);
}

//--------------------------------------------------------------------------------

module_init(_init_module);
module_exit(_cleanup_module);
