/** @file ksample02.c
*
* @author J. Krauth
* @author Hilpert Rodolphe
*
* Simple kernel sample with DMA interrupt.
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

#include <xpci3xxx.h>
#include <xpci3xxx-kapi.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ADDI-DATA");
MODULE_DESCRIPTION("XPCI-3XXX Analog input DMA sample");
MODULE_SUPPORTED_DEVICE("XPCI-3XXX");

EXPORT_NO_SYMBOLS;

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

static int parmNbrChannels = 2;
static int parmNbrSequences = 100;
static int parmNbrSeqPerIRQ = 2;
static int parmDMA = 1;
static int parmConvertTime = 10;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
	MODULE_PARM (parmNbrChannels,"i");
	MODULE_PARM (parmNbrSequences, "i");
	MODULE_PARM (parmNbrSeqPerIRQ, "i");
	MODULE_PARM (parmDMA, "i");
	MODULE_PARM (parmConvertTime, "i");
#else
	module_param(parmNbrChannels, int, S_IRUGO);
	module_param(parmNbrSequences, int, S_IRUGO);
	module_param(parmNbrSeqPerIRQ, int, S_IRUGO);
	module_param(parmDMA, int, S_IRUGO);
	module_param(parmConvertTime, int, S_IRUGO);
#endif

MODULE_PARM_DESC (parmNbrChannels, "Number of channels (default: 2)");
MODULE_PARM_DESC (parmNbrSequences, "Number of sequences (default: 10)");
MODULE_PARM_DESC (parmNbrSeqPerIRQ, "Number of sequences per interrupt (default: 2)");
MODULE_PARM_DESC (parmDMA, "Use DMA (default: 1, 1:Yes, 0: No)");
MODULE_PARM_DESC (parmConvertTime, "Convert time (default: 5, 5 - 65535)");

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

#define MODULE_NAME "ksample02"

#define TOTAL_NUMBER_OF_INTERRUPT				(parmNbrSequences / parmNbrSeqPerIRQ)
#define NUMBER_OF_ACQUISITION_PRO_INTERRUPT		(parmNbrChannels * parmNbrSeqPerIRQ)

#define ANALOG_INPUT_FIFO_EMPTY			0xfffffffe
#define ANALOG_INPUT_FIFO_OVERFLOW		0xffffffff
#define ANALOG_INPUT_WITH_DMA			1
#define ANALOG_INPUT_WITHOUT_DMA		2

#define NUMBER_OF_BOARDS	1

#define DRIVER_FIFO_STATUS				0
#define DRIVER_FIFO_SOURCE				0
#define NBR_OF_DMA_BUFFER 	(2 + ADDIDATA_MAX_AI)
#define NBR_OF_VALUE		(3 + ADDIDATA_MAX_AI)
#define FISRT_VALUE_ADDRESS	(4 + ADDIDATA_MAX_AI)

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

static struct pci_dev * _pdev[NUMBER_OF_BOARDS];
static int gIRQCpt;
static uint16_t *gBuff;
static uint32_t *gInfo;
static uint8_t  gChannelList[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

/* The user callback interrupt */
static void interrupt_callback (struct pci_dev * pdev, uint32_t dw_Dummy)
{
	uint32_t minor;
	uint8_t valueCpt;
	uint16_t *value;
	uint32_t timer = 0;

		i_xpci3xxx_ReadTimerValue (pdev, 0, &timer);

		/* No more acquisitions have to be done in this sample */
		// if (gIRQCpt >= TOTAL_NUMBER_OF_INTERRUPT)
		//	return;

		/* Get shared memory pointer (pointer on DMA value)
		 * minor: Minor number of the card from which the informations are read.
		 * gInfo: Number of buffer and number of value for each buffer, buffer address.
		 *
		 *        gInfo[0] DRIVER_FIFO_SOURCE or DRIVER_FIFO_STATUS
		 *                 It contains the driver status ANALOG_INPUT_FIFO_EMPTY or ANALOG_INPUT_FIFO_OVERFLOW
		 *                 When the FIFO is not full or empty, gInfo[0] contains the interruption source ANALOG_INPUT_WITH_DMA
		 *                 or ANALOG_INPUT_WITHOUT_DMA.
		 *
		 *        gInfo[2 + 16] NBR_OF_DMA_BUFFER
		 *                 It contains the number of DMA buffer.
		 *
		 *        gInfo[3 + 16]
		 *                 It contains the DMA buffer size (number of values in the buffer).
		 *
		 *        gInfo[4 + 16]
		 *                 It contains the DMA buffer address offset.
		 *
		 * value: It contains the pointer on the shared memory.
		 */
		i_xpci3xxx_TestInterrupt(pdev, &minor, gInfo, &value);

		/* Test if FIFO Empty */
		if (ANALOG_INPUT_FIFO_EMPTY != gInfo[DRIVER_FIFO_STATUS])
		{
			/* Analog input with DMA */
			if (gInfo[DRIVER_FIFO_SOURCE] & ANALOG_INPUT_WITH_DMA)
			{
				/* Test if there is just one buffer. (The Linux driver managed one buffer) */
				if (gInfo[NBR_OF_DMA_BUFFER] == 1)
				{
					for (valueCpt = 0 ; valueCpt < gInfo[NBR_OF_VALUE] ; valueCpt ++)
					{
						/*
						 * value: It contains the pointer on the shared memory.
						 * valueCpt: Increment the shared memory pointer address to get the corresponding value.
						 * gInfo[FISRT_VALUE_ADDRESS]: Offset of the DMA memory.
						 */
						gBuff[(gIRQCpt * NUMBER_OF_ACQUISITION_PRO_INTERRUPT) + valueCpt] = *(value + valueCpt + gInfo[FISRT_VALUE_ADDRESS]);
					}

					printk ("Read timer value %u usec.: %u sec.: %u\n", timer, gInfo[FISRT_VALUE_ADDRESS + 1], gInfo[FISRT_VALUE_ADDRESS + 2]);
				}
			}

			/* Analog input without DMA */
			if (gInfo[DRIVER_FIFO_SOURCE] & ANALOG_INPUT_WITHOUT_DMA)
			{
				for (valueCpt = 0 ; valueCpt < NUMBER_OF_ACQUISITION_PRO_INTERRUPT ; valueCpt ++)
					gBuff[(gIRQCpt * NUMBER_OF_ACQUISITION_PRO_INTERRUPT) + valueCpt] = gInfo[1 + valueCpt];
			}
		}

		/* Test if FIFO Full */
		if (ANALOG_INPUT_FIFO_OVERFLOW == gInfo[DRIVER_FIFO_STATUS])
			printk ("ANALOG_INPUT_FIFO_OVERFLOW\n");

		gIRQCpt++;
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

static void initTimer(struct pci_dev * pdev)
{
	int error = 0;
	unsigned long irqstate;

		xpci3xxx_lock(pdev,&irqstate);
		{
			error = i_xpci3xxx_InitTimer (pdev,
											0,		// Timer number
											2,		// Timer mode
											1,		// Time unit us
											1000);	// Reload value (wait 500ms)
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
			error = i_xpci3xxx_EnableDisableTimerInterrupt (pdev,
															0,					// Timer 0
															ADDIDATA_DISABLE);	// Interrupt disable
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
}

//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

static int ksample02_init(struct pci_dev * pdev)
{
	int error = 0;
	unsigned long irqState;
    uint8_t  gainList[16] = {ADDIDATA_1_GAIN,
								ADDIDATA_1_GAIN,
								ADDIDATA_1_GAIN,
								ADDIDATA_1_GAIN,
								ADDIDATA_1_GAIN,
								ADDIDATA_1_GAIN,
								ADDIDATA_1_GAIN,
								ADDIDATA_1_GAIN,
								ADDIDATA_1_GAIN,
								ADDIDATA_1_GAIN,
								ADDIDATA_1_GAIN,
								ADDIDATA_1_GAIN,
								ADDIDATA_1_GAIN,
								ADDIDATA_1_GAIN,
								ADDIDATA_1_GAIN,
								ADDIDATA_1_GAIN};

    uint8_t  polarityList[16] = {ADDIDATA_UNIPOLAR,
								ADDIDATA_UNIPOLAR,
								ADDIDATA_UNIPOLAR,
								ADDIDATA_UNIPOLAR,
								ADDIDATA_UNIPOLAR,
								ADDIDATA_UNIPOLAR,
								ADDIDATA_UNIPOLAR,
								ADDIDATA_UNIPOLAR,
								ADDIDATA_UNIPOLAR,
								ADDIDATA_UNIPOLAR,
								ADDIDATA_UNIPOLAR,
								ADDIDATA_UNIPOLAR,
								ADDIDATA_UNIPOLAR,
								ADDIDATA_UNIPOLAR,
								ADDIDATA_UNIPOLAR,
								ADDIDATA_UNIPOLAR};

		printk ("Analog input sequence (DMA and interrupt)\n");

		if ((parmNbrChannels & 1) && (parmNbrSeqPerIRQ < 2))
		{
			printk ("The number of sequnces per interrupt has to be >=2\n");
			return 1;
		}

		xpci3xxx_lock(pdev,&irqState);
		{
			error = i_xpci3xxx_InitAnalogInputSequence (pdev,
														(uint8_t)parmNbrChannels,
														gChannelList,
														gainList,
														polarityList,
														ADDIDATA_SINGLE,
														(uint32_t)parmNbrSequences,										// Number of sequence (0: Continuous mode)
														(uint8_t)((parmDMA==1)?ADDIDATA_ENABLE:ADDIDATA_DISABLE),
														(uint32_t)parmNbrSeqPerIRQ,										// Number of sequence for each interrupt
														ADDIDATA_DISABLE,			// Delay mode
														0,							// Delay time unit
														0);							// Delay time
		}
		xpci3xxx_unlock(pdev,irqState);
	
		printk ("i_xpci3xxx_InitAnalogInputSequence :");
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
			case 7: printk ("Wrong use DMA parameter\n");
			break;
			case 8: printk ("Wrong number of sequence for each interrupt parameter\n");
			break;
			case 9: printk ("Wrong delay mode parameter\n");
			break;
			case 10: printk ("Wrong delay time unit parameter\n");
			break;
			case 11: printk ("Wrong delay time parameter\n");
			break;
			case 12: printk ("A conversion is already started\n");
			break;
			case 13: printk ("DMA mode not supported\n");
			break;	
			case 14: printk ("No memory space available\n");
			break;	
			case 15: printk ("Error by initialising the DMA\n");
			break;	

			default: printk ("Error %i\n", error);
		}

		if (error)
			return 1;

		gInfo = kmalloc (sizeof(uint32_t) * (NBR_OF_DMA_BUFFER + ADDIDATA_MAX_INTERRUPT_ANALOG_INPUT_VALUE), GFP_KERNEL);
		if (gInfo == NULL)
			return 1;

		gBuff = kmalloc (sizeof(uint16_t) * TOTAL_NUMBER_OF_INTERRUPT * NUMBER_OF_ACQUISITION_PRO_INTERRUPT, GFP_KERNEL);
		if (gBuff == NULL)
			return 1;

		xpci3xxx_lock(pdev,&irqState);
		{
			error = i_xpci3xxx_StartAnalogInputSequence (pdev,
														ADDIDATA_MICRO_SECOND,	// Convert time unit
														parmConvertTime);		// Convert time
		}
		xpci3xxx_unlock(pdev,irqState);

		printk ("i_xpci3xxx_StartAnalogInputSequence :");
		switch (error) 
		{
			case 0: printk ("OK\n");
			break;
			case 1: printk ("The handle parameter of the board is wrong\n");
			break;
			case 2: printk ("Sequence not initialised\n");
			break;
			case 3: printk ("Wrong convert time unit\n");
			break;
			case 4: printk ("Wrong convert time\n");
			break;
			case 5: printk ("A conversion is already started\n");
			break;
			case 6: printk ("Error by starting DMA\n");
			break;

			default: printk ("Error %i\n", error);
		}
	
	return error;
}

//--------------------------------------------------------------------------------

static void ksample02_release(struct pci_dev * pdev)
{
	int error = 0;
	unsigned long irqState;

		/*
		* Remark: It is possible that error 3 occurred. 
		* That depends of the initialization type.
		* If e.g. only 2 DMA with interrupt acquisitions have
		* to be done, at the end of the second acquisition,
		* the "Sequence Started" flag is reset. The work
		* of i_xpci3xxx_StopAnalogInputSequence is done
		* in the interrupt function. Don't care about this
		* error.
		*/
		xpci3xxx_lock(pdev,&irqState);
		{
			error = i_xpci3xxx_StopAnalogInputSequence (pdev);
		}
		xpci3xxx_unlock(pdev,irqState);
	
		printk ("i_xpci3xxx_StopAnalogInputSequence :");
		switch (error) 
		{
			case 0: printk ("OK\n");
			break;
			case 1: printk ("The handle parameter of the board is wrong\n");
			break;
			case 2: printk ("Sequence not initialised\n");
			break;
			case 3: printk ("Sequence not started\n");
			break;
			case 4: printk ("Error by stoping DMA\n");
			break;

			default: printk ("Error %i\n", error);
		}

		xpci3xxx_lock(pdev,&irqState);
		{
			error = i_xpci3xxx_ReleaseAnalogInputSequence (pdev);
		}
		xpci3xxx_unlock(pdev,irqState);
	
		printk ("i_xpci3xxx_ReleaseAnalogInputSequence :");
		switch (error) 
		{
			case 0: printk ("OK\n");
			break;
			case 1: printk ("The handle parameter of the board is wrong\n");
			break;
			case 2: printk ("Sequence not initialised\n");
			break;
			case 3: printk ("A conversion is already started\n");
			break;
			case 4: printk ("Error by releasing DMA\n");
			break;

			default: printk ("Error %i\n", error);
		}
}

//--------------------------------------------------------------------------------

/** Called when the driver is loaded.*/
static int __init _init_module(void)
{
	int index = 0; /* Index of the board to use (here the fisrt board) */
	unsigned long irqState;
	
		gIRQCpt = 0;

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
		
		/* Set the user interrupt callback */
		xpci3xxx_SetIntCallback (interrupt_callback);

		/* Test the first board found */
		for (index=0; index<NUMBER_OF_BOARDS; index++)
		{
			initTimer(_pdev[index]);

			if ( ksample02_init (_pdev[index]) )
			{
				/* Reset the user interrupt callback */
				xpci3xxx_lock(_pdev[index],&irqState);
				{
					xpci3xxx_ResetIntCallback ();
				}
				xpci3xxx_unlock(_pdev[index],irqState);
				return -EIO;
			}
		}
		
		return 0;
}

//--------------------------------------------------------------------------------

/** Called when the driver is unloaded.*/
static void __exit _cleanup_module(void)
{
	int irq = 0;
	int valueIndex = 0;
	int chIndex = 0;
	int seqIndex = 0;
	int seqIRQIndex = 0;
	
		for (valueIndex=0; valueIndex<NUMBER_OF_BOARDS; valueIndex++)
			ksample02_release (_pdev[valueIndex]);
			
		/* Reset the user interrupt callback */
		xpci3xxx_ResetIntCallback ();

		printk ("parmNbrChannels=%i parmNbrSequences=%i parmNbrSeqPerIRQ=%i parmDMA=%i\n", parmNbrChannels, parmNbrSequences, parmNbrSeqPerIRQ, parmDMA);

		for (irq=0; irq < gIRQCpt; irq++)
		{
			valueIndex = 0;

			for (seqIRQIndex=0; seqIRQIndex<parmNbrSeqPerIRQ; seqIRQIndex++)
			{
				for (chIndex=0; chIndex<parmNbrChannels; chIndex++)
					printk ("IRQ:%i\tSEQ:%i\tCH:%hu=%4hx\n", irq, seqIndex, gChannelList[chIndex], gBuff[irq * NUMBER_OF_ACQUISITION_PRO_INTERRUPT + valueIndex++]);

				seqIndex++;
			}
		}
		
		if (gBuff!=NULL)
			kfree(gBuff);

		if (gInfo!=NULL)
			kfree(gInfo);
}

//--------------------------------------------------------------------------------

module_init(_init_module);
module_exit(_cleanup_module);
