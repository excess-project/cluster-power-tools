/** @file interrupt.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
*
* @version  $LastChangedRevision$
* @version $LastChangedDate$
*
* This module implements the interrupt related functions.
*
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

#include "xpci3xxx-private.h"
#include "interrupt.h"

EXPORT_SYMBOL (xpci3xxx_SetIntCallback);
EXPORT_SYMBOL (xpci3xxx_ResetIntCallback);
EXPORT_SYMBOL (i_xpci3xxx_TestInterrupt);

/**@def EXPORT_NO_SYMBOLS
 * Function in this file are not exported.
 */
EXPORT_NO_SYMBOLS;

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

#include <linux/time.h>

//------------------------------------------------------------------------------
/* global interrupt call back */
void (*xpci3xxx_private_InterruptCallback) (struct pci_dev * pdev, uint32_t dw_InterruptSource) = NULL;

//------------------------------------------------------------------------------
/**  */
int xpci3xxx_SetIntCallback( void(*InterruptCallback) (struct pci_dev * pdev, uint32_t dw_InterruptSource))
{
	xpci3xxx_private_InterruptCallback = InterruptCallback;
    return 0;
}

//------------------------------------------------------------------------------

int
xpci3xxx_ResetIntCallback(void)
{
	xpci3xxx_private_InterruptCallback = NULL;
	return 0;
}

// Is used to read the interrupt jitter
int i_xpci3xxx_FastReadTimerValue (struct pci_dev *pdev, uint32_t* value);

/** Time-stamp management.
 *
 * Compute the time-stamp regarding elapsed time since the interrupt (jitter).
 */
static __inline__ void computeTimeStamp (struct pci_dev *pdev, struct timespec *timestamp)
{
	uint32_t irqJitter = 0;
	long toSub = 0;
	struct timespec currentTime;

	// Get the time
	getnstimeofday (&currentTime);

	// Get the interrupt jitter
	i_xpci3xxx_FastReadTimerValue (pdev, &irqJitter);

	// This is the interrupt jitter time
	toSub = (XPCI3XXX_PRIVDATA(pdev)->dw_ReloadValue - irqJitter);

	// If the current time is > to the time to subtract
	if (currentTime.tv_nsec >= (toSub * 1000))
	{
		// Do the subtraction
		timestamp->tv_sec = currentTime.tv_sec;
		timestamp->tv_nsec = (currentTime.tv_nsec - (toSub * 1000));
	}
	else
	{
		// Remove one second
		timestamp->tv_sec = (currentTime.tv_sec - 1);

		// Add it to the microseconds to subtract the jitter times
		timestamp->tv_nsec = (currentTime.tv_nsec + (1000000000 - (toSub * 1000)));

		// Check if we have enough microseconds to get a second because of adding the jitter time.
		while (timestamp->tv_nsec >= 1000000000)
		{
			timestamp->tv_nsec -= 1000000000;
			timestamp->tv_sec++;
		}
	}
}

/**
 *
 * Interrupt function for the timer.
 *
 * @param[in] pdev       : PCI Device struct         \n
 *
 * @param[out] pdw_InterruptSource : 0x10 : Timer 0            \n
 *                                                   0x20 : Timer 1            \n
 *                                                   0x40 : Timer 2            \n
 *                                                   and so on...              \n
 * @param[out] pb_HasGenerateInterrupt :                        \n
 *                                  0 : this functionality has not generate the\n
 *                                      interrupt                              \n
 *                                  1 : this functionality has generate the    \n
 *                                      interrupt                              \n
 *
 * @return  -                                                      \n
 */
void v_xpci3xxx_TimerInterruptFunction
                             (struct pci_dev *pdev,
                               uint32_t* pdw_InterruptSource,
                               uint8_t* pb_HasGenerateInterrupt)
{
        /* Variable declaration */
        uint32_t dw_StatusRegisterTimer = 0;
        uint32_t dw_BaseAdress = 0;
		uint8_t   b_TWCCounter = 0;

		*pb_HasGenerateInterrupt = 0;
		*pdw_InterruptSource = 0;

        for (b_TWCCounter = 0 ; b_TWCCounter < ADDIDATA_MAX_TCW ; b_TWCCounter ++)
       	{
        	if (XPCI3XXX_PRIVDATA(pdev)->b_TimerCounterWatchdogConfigurableAsTimer[b_TWCCounter])
           	{
				if (XPCI3XXX_PRIVDATA(pdev)->b_TimerCounterWatchdogConfiguration[b_TWCCounter] == ADDIDATA_TIMER)
				{
					if (pdev->device != xpci3501_BOARD_DEVICE_ID)
						dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TWCCounter];
					else
						dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TWCCounter];

					/* Is the interrupt enabled? */
					INPDW(dw_BaseAdress + 12, &dw_StatusRegisterTimer);

					if (dw_StatusRegisterTimer & 0x2)
					{
						INPDW(dw_BaseAdress + 20, &dw_StatusRegisterTimer);
						dw_StatusRegisterTimer = dw_StatusRegisterTimer & 0x1;
					}

					/*dw_StatusRegister = READ_BIT_MASK   (b_CallLevel,
					                                        &ps_SingleTimerProcessStruct->
					                                        s_HardwareInformation,
					                                        20,
					                                        0x1UL,
					                                        &i_ErrorCodeRBM);*/
					if (dw_StatusRegisterTimer == 0x1)
					{
						/* This functionality has generate the interrupt */
						*pb_HasGenerateInterrupt = 1;
						*pdw_InterruptSource |= 0x10 << b_TWCCounter;
					}
				}
           	}
       	}
}

/**
 *
 * Interrupt function for the counter.
 *
 * @param[in] pdev       : PCI Device struct         \n
 *
 * @param[out] pdw_InterruptSource : 0x100 : Counter 0         \n
 *                                                   0x200 : Counter 1         \n
 *                                                   0x400 : Counter 2         \n
 *                                                   and so on...              \n
 * @param[out] pb_HasGenerateInterrupt :                        \n
 *                                  0 : this functionality has not generate the\n
 *                                      interrupt                              \n
 *                                  1 : this functionality has generate the    \n
 *                                      interrupt                              \n
 *
 * @return  -                                                      \n
 */
void v_xpci3xxx_CounterInterruptFunction
                             (struct pci_dev *pdev,
                               uint32_t* pdw_InterruptSource,
                               uint8_t* pb_HasGenerateInterrupt)
{
        /* Variable declaration */
        uint32_t dw_StatusRegisterCounter = 0;
        uint32_t dw_BaseAdress = 0;
		uint8_t   b_TWCCounter = 0;

		*pb_HasGenerateInterrupt = 0;
		*pdw_InterruptSource = 0;

        for (b_TWCCounter = 0 ; b_TWCCounter < ADDIDATA_MAX_TCW ; b_TWCCounter ++)
       	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
                b_TimerCounterWatchdogConfigurableAsCounter[b_TWCCounter])
           	{
				if (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TWCCounter] == ADDIDATA_COUNTER)
				{
					dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TWCCounter];

					INPDW(dw_BaseAdress + 20, &dw_StatusRegisterCounter);
					dw_StatusRegisterCounter = dw_StatusRegisterCounter & 0x1;

					if (dw_StatusRegisterCounter == 0x1)
					{
						/* This functionality has generate the interrupt */
						*pb_HasGenerateInterrupt = 1;
						*pdw_InterruptSource |= 0x100 << b_TWCCounter;
					}
				}
            }
        }

}

/**
 *
 * Interrupt function for the watchdog.
 *
 * @param[in] pdev       : PCI Device struct         \n
 *
 * @param[out] pdw_InterruptSource : 0x1000 : Watchdog 0       \n
 *                                                   0x2000 : Watchdog 1       \n
 *                                                   0x4000 : Watchdog 2       \n
 *                                                   and so on...              \n
 * @param[out] pb_HasGenerateInterrupt :                        \n
 *                                  0 : this functionality has not generate the\n
 *                                      interrupt                              \n
 *                                  1 : this functionality has generate the    \n
 *                                      interrupt                              \n
 *
 * @return  -                                                      \n
 */
void v_xpci3xxx_WatchdogInterruptFunction
                             (struct pci_dev *pdev,
                               uint32_t* pdw_InterruptSource,
                               uint8_t* pb_HasGenerateInterrupt)
{
        /* Variable declaration */
        uint32_t dw_StatusRegisterWatchdog = 0;
        uint32_t dw_BaseAdress = 0;
		uint8_t   b_TWCCounter = 0;

		*pb_HasGenerateInterrupt = 0;
		*pdw_InterruptSource = 0;

        for (b_TWCCounter = 0 ; b_TWCCounter < ADDIDATA_MAX_TCW ; b_TWCCounter ++)
       	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
                b_TimerCounterWatchdogConfigurableAsWatchdog[b_TWCCounter])
            {
				if (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TWCCounter] == ADDIDATA_WATCHDOG)
				{
					if (pdev->device != xpci3501_BOARD_DEVICE_ID)
						dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TWCCounter];
					else
						dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TWCCounter];

					INPDW(dw_BaseAdress + 20, &dw_StatusRegisterWatchdog);
					dw_StatusRegisterWatchdog = dw_StatusRegisterWatchdog & 0x1;

					if (dw_StatusRegisterWatchdog == 0x1)
					{
						/* This functionality has generate the interrupt */
						*pb_HasGenerateInterrupt = 1;
						*pdw_InterruptSource |= 0x1000 << b_TWCCounter;
					}
				}
            }
        }
}

/**
 *
 * Interrupt function for the analog input.
 *
 * @param[in] pdev       : PCI Device struct         \n
 *
 * @param[out] pdw_InterruptSource : 0x1 : IRQ with DMA        \n
 *                                                   0x2 : IRQ without DMA     \n
 *                                                   0x4 : FIFO Overflow       \n
 * @param[out] pdw_AnalogInputValue : Table of the             \n
 *                                                       analog input value    \n
 * @param[out] pb_HasGenerateInterrupt :                        \n
 *                                  0 : this functionality has not generate the\n
 *                                      interrupt                              \n
 *                                  1 : this functionality has generate the    \n
 *                                      interrupt                              \n
 *
 * @return  -                                                      \n
 */
void v_xpci3xxx_AnalogInputInterruptFunction
                             (struct pci_dev *pdev,
                               uint32_t* pdw_InterruptSource,
                               uint32_t* pdw_AnalogInputValue,
                               uint8_t* pb_HasGenerateInterrupt)
{
        /* Variable declaration */
        uint32_t dw_StatusRegisterAnalogInput = 0;
		uint8_t  b_SequenceChannelCounter = 0;
        uint32_t dw_ChannelNumber = 0;
        uint32_t dw_SetMask = 0;
        uint32_t dw_ResetMask = 0;
        uint32_t dw_WriteValueDWord = 0;

        uint32_t dw_MemoryAddress = 0;
        uint32_t dw_ScatterGatherIndex = 0;
        uint32_t dw_SequenceArray [ADDIDATA_MAX_INTERRUPT_ANALOG_INPUT_VALUE];
        uint32_t dw_Counter = 0;

		*pb_HasGenerateInterrupt = 0;
		*pdw_InterruptSource = 0;

        /*Read the interrupt status register*/
		INPDW_MEM(GET_MEM_BAR3(pdev), 16, &dw_StatusRegisterAnalogInput);

		/* Test if EOS irq */
		if (dw_StatusRegisterAnalogInput & 0x3UL)
		{
			/* Clear the interrupt flag */
			dw_SetMask = dw_StatusRegisterAnalogInput & 0x3UL;
			dw_ResetMask = 0xFFFFFFFFUL;
			INPDW_MEM(GET_MEM_BAR3(pdev), 16, &dw_WriteValueDWord);
			dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
			OUTPDW_MEM(GET_MEM_BAR3(pdev), 16, dw_WriteValueDWord);

			/*Read the channel number*/
			INPDW_MEM(GET_MEM_BAR3(pdev), 24, &dw_ChannelNumber);
			dw_ChannelNumber = dw_ChannelNumber & 0xFFFFFFFFUL;

			for (b_SequenceChannelCounter = 0 ; b_SequenceChannelCounter < XPCI3XXX_PRIVDATA(pdev)->b_NumberOfSequenceChannels ; b_SequenceChannelCounter ++)
			{
	            /*Read the digital analog input value*/
				INPDW_MEM(GET_MEM_BAR3(pdev), 28, &pdw_AnalogInputValue[b_SequenceChannelCounter]);
				pdw_AnalogInputValue[b_SequenceChannelCounter] = pdw_AnalogInputValue[b_SequenceChannelCounter] & 0xFFFFFFFFUL;
			}


			/* Read the number of sequence */
			INPDW_MEM(GET_MEM_BAR3(pdev), 48, &pdw_AnalogInputValue[XPCI3XXX_PRIVDATA(pdev)->b_NumberOfSequenceChannels]);

			/* This functionality has generate the interrupt */
			*pb_HasGenerateInterrupt = 1;
			*pdw_InterruptSource = 0x2;
		}
		else
		{
			/* Test if DMA irq */
			if (dw_StatusRegisterAnalogInput & 0x14UL)
			{
				/* Clear the interrupt flag */
				dw_SetMask = 0x4UL;
				dw_ResetMask = 0xFFFFFFFFUL;
				INPDW_MEM(GET_MEM_BAR3(pdev), 16, &dw_WriteValueDWord);
				dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
				OUTPDW_MEM(GET_MEM_BAR3(pdev), 16, dw_WriteValueDWord);

		       	/* Clear the PCI DMA interrupt flag */
			   	dw_SetMask = (0x3 | 0x8) << (8 * XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor->b_DMAChannel);
			   	dw_ResetMask = 0xFFFFFFFFUL;
			   	INPDW_MEM(GET_MEM_BAR0(pdev), 0xA8, &dw_WriteValueDWord);
			   	dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
			   	OUTPDW_MEM(GET_MEM_BAR0(pdev), 0xA8, dw_WriteValueDWord);

           		/* Get the actual memory address */
				INPDW_MEM(GET_MEM_BAR0(pdev), 0x84 + (0x14 * XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor->b_DMAChannel), &dw_MemoryAddress);

	            /* Variable initialization */
	            dw_SequenceArray [0]  = 0;
	            dw_ScatterGatherIndex = XPCI3XXX_PRIVDATA(pdev)->
	                                   dw_LastScatterGatherBuffer;

	            for (dw_Counter = 0; dw_Counter < XPCI3XXX_PRIVDATA(pdev)->
	                			                  ps_ScatterGatherDescriptor->
	                                              dw_NumberOfScatterGatherBuffer;
	                dw_Counter ++,
	                dw_ScatterGatherIndex = (dw_ScatterGatherIndex + 1) % XPCI3XXX_PRIVDATA(pdev)->
									                                      ps_ScatterGatherDescriptor->
									                                      dw_NumberOfScatterGatherBuffer)
            	{
	               	/* Get all scatter gather buffer and call user function */
	               	if ((XPCI3XXX_PRIVDATA(pdev)->
	              	   ps_ScatterGatherDescriptor->
	                   s_ScatterGatherSingleBuffer [dw_ScatterGatherIndex].
	                   dw_PhysicalAddress != dw_MemoryAddress) ||
	                  ((XPCI3XXX_PRIVDATA(pdev)->
	              	    ps_ScatterGatherDescriptor->
	                    s_ScatterGatherSingleBuffer [dw_ScatterGatherIndex].
	                    dw_PhysicalAddress == dw_MemoryAddress) &&
	                   (XPCI3XXX_PRIVDATA(pdev)->
	              	    ps_ScatterGatherDescriptor->
	                    b_AcquisitionMode == 0) &&
	                   ((dw_ScatterGatherIndex + 1) == XPCI3XXX_PRIVDATA(pdev)->
	              	   								   ps_ScatterGatherDescriptor->
	                                                   dw_NumberOfScatterGatherBuffer) &&
	                   (dw_StatusRegisterAnalogInput & 0x10)))
               		{
	                 	/* Save the buffer size in number of elements (not in byte!) */
	                 	dw_SequenceArray [1 + (dw_SequenceArray [0] * 6)] = XPCI3XXX_PRIVDATA(pdev)->
														                 	ps_ScatterGatherDescriptor->
														                 	s_ScatterGatherSingleBuffer [dw_ScatterGatherIndex].
														                 	dw_Size / 2;

						/* Save the offset address */
						dw_SequenceArray [2 + (dw_SequenceArray [0] * 6)] = (uint32_t)((uint32_t)((uint32_t) XPCI3XXX_PRIVDATA(pdev)->
																											 ps_ScatterGatherDescriptor->
																											 s_ScatterGatherSingleBuffer [dw_ScatterGatherIndex].
																											 s_Buffer.
																											 pb_DMABuffer - (uint32_t) XPCI3XXX_PRIVDATA(pdev)->
																																	   ps_ScatterGatherDescriptor->
																																	   s_ScatterGatherSingleBuffer [0].
																																	   s_Buffer.
																																	   pb_DMABuffer) / 2);

	                 	/* Save the time stamp */
						{
							struct timespec timestamp;
							computeTimeStamp (pdev, &timestamp);
							dw_SequenceArray [3 + (dw_SequenceArray [0] * 6)] = (timestamp.tv_nsec & 0xFFFFFFFF);
							dw_SequenceArray [4 + (dw_SequenceArray [0] * 6)] = (timestamp.tv_nsec & 0xFFFFFFFF00000000) >> 32;
							dw_SequenceArray [5 + (dw_SequenceArray [0] * 6)] = (timestamp.tv_sec & 0xFFFFFFFF);
							dw_SequenceArray [6 + (dw_SequenceArray [0] * 6)] = (timestamp.tv_sec & 0xFFFFFFFF00000000) >> 32;
						}

						/* Increment the number of buffer */
	                 	dw_SequenceArray [0] = dw_SequenceArray [0] + 1;

	                 	/* Test if must call user function */
	                 	if (XPCI3XXX_PRIVDATA(pdev)->
	                    	 ps_ScatterGatherDescriptor->
	                    	 s_ScatterGatherSingleBuffer [dw_ScatterGatherIndex].
	                    	 b_Interrupt == 1)
                    	{
				            /* Copy the informations in the FIFO */
							if (((XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterWrite + 1) % ADDIDATA_MAX_EVENT_COUNTER) != XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterRead)
							{
								XPCI3XXX_PRIVDATA(pdev)->dw_InterruptSource[XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterWrite] = 0x1;

								memcpy(XPCI3XXX_PRIVDATA(pdev)->dw_SequenceArray[XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterWrite], dw_SequenceArray, sizeof(dw_SequenceArray));

								XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterWrite = (XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterWrite + 1) % ADDIDATA_MAX_EVENT_COUNTER;

//								printk ("%s %i WR %u RD %u\n", __FUNCTION__, __LINE__, XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterWrite, XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterRead);
							}
							else
							{
								XPCI3XXX_PRIVDATA(pdev)->dw_InterruptSource[XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterWrite] = 0xffffffff;
							}

		                    /* Test if the last acquisition */
		                    if ((XPCI3XXX_PRIVDATA(pdev)->
		                    	 ps_ScatterGatherDescriptor->
		                         b_AcquisitionMode == 0) &&
		                        (dw_StatusRegisterAnalogInput & 0x10))
	                    	{
		                       	/* Disable the module sequence transfer*/

								/* Disable the interrupt */
								/* Disable the DMA       */
								/* Stop the acquisition  */
								dw_SetMask = 0x0UL;
								dw_ResetMask = 0x380000UL;
								INPDW_MEM(GET_MEM_BAR3(pdev), 8, &dw_WriteValueDWord);
								dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
								OUTPDW_MEM(GET_MEM_BAR3(pdev), 8, dw_WriteValueDWord);

								/* Clear all DMA interrupt source */
								dw_SetMask = 0x35UL;
								dw_ResetMask = 0xFFFFFFFFUL;
								INPDW_MEM(GET_MEM_BAR3(pdev), 16, &dw_WriteValueDWord);
								dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
								OUTPDW_MEM(GET_MEM_BAR3(pdev), 16, dw_WriteValueDWord);

	                          	/* Reset the DMA PCI controller initialisation */

								/* Disable the DMA transfer interrupt */
								v_xpci3xxx_Disable9054DMATransferInterrupt  (pdev);

								/* Abort the DMA transfer */
								v_xpci3xxx_Abort9054DMATransfer  (pdev);

								/* Stop the DMA transfer */
								v_xpci3xxx_Pause9054DMATransfer  (pdev);

								/* Reset the descriptor list */
								v_xpci3xxx_Reset9054DescriptorList  (pdev);

		                       	/* Clear the DMA counter interrupt flag */
							   	dw_SetMask = 0x10;
							   	dw_ResetMask = 0xFFFFFFFFUL;
							   	INPDW_MEM(GET_MEM_BAR3(pdev), 16, &dw_WriteValueDWord);
							   	dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
							   	OUTPDW_MEM(GET_MEM_BAR3(pdev), 16, dw_WriteValueDWord);

								XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputSequenceStarted = ADDIDATA_DISABLE;

		                       	break;

	                    	}
		                    dw_SequenceArray [0] = 0;

		                    XPCI3XXX_PRIVDATA(pdev)->
		                    dw_LastScatterGatherBuffer = (dw_ScatterGatherIndex + 1) %
		                                                  XPCI3XXX_PRIVDATA(pdev)->
		                                                  ps_ScatterGatherDescriptor->
		                                                  dw_NumberOfScatterGatherBuffer;

						} // end of if (XPCI3XXX_PRIVDATA(pdev)->...b_Interrupt == 1)
					} // end of if ((... dw_PhysicalAddress != dw_MemoryAddress) || ((...dw_PhysicalAddress == dw_MemoryAddress) && (...b_AcquisitionMode == 0) && ((dw_ScatterGatherIndex + 1) == ...dw_NumberOfScatterGatherBuffer) && (dw_StatusRegisterAnalogInput & 0x10)))
	               	else
					{
						break;
					}
				}

				/* This functionality has generate the interrupt */
				*pb_HasGenerateInterrupt = 1;
				*pdw_InterruptSource = 0x1;
			}
			else
			{
				/* Test if FIFO DMA Overflow irq */
				if (dw_StatusRegisterAnalogInput & 0x20UL)
				{
					/* Clear the interrupt flag */
					dw_SetMask = dw_StatusRegisterAnalogInput & 0x20UL;
					dw_ResetMask = 0xFFFFFFFFUL;
					INPDW_MEM(GET_MEM_BAR3(pdev), 16, &dw_WriteValueDWord);
					dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
					OUTPDW_MEM(GET_MEM_BAR3(pdev), 16, dw_WriteValueDWord);

					/* This functionality has generate the interrupt */
					*pb_HasGenerateInterrupt = 1;
					*pdw_InterruptSource = 0x4;

					printk ("%s %i FIFO DMA Overflow irq\n", __FUNCTION__, __LINE__);
				}
			}
		}
	}

/** IRQ handler prototype has changed between 2.4 and 2.6.
 * in 2.4 the handler has not return value, in 2.6 it is of type irqreturn_t
 * IRQ_HANDLED means that the device did indeed need attention from the driver
 * IRQ_NONE means that the device didn't actually need attention
 *
 * NOTE: the change between 2.4 and 2.6 was not so important that it needed
 * two version of the function. BUT if in the course of the implementation you
 * notice the changes are so important that maintaining a code for both version
 * in one function is just a hassle, DON'T HESITATE and create two versions
 * of the same function.
 *
 */

//------------------------------------------------------------------------------
/** IRQ Handler */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)	/* 2.4  */
	#define RETURN_HANDLED return
	#define RETURN_NONE return
#else
	#define RETURN_HANDLED return IRQ_HANDLED
	#define RETURN_NONE return IRQ_NONE
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)	/* 2.4  */
	static void xpci3xxx_do_interrupt(int irq, void * dev_id, struct pt_regs *regs)
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)       /* 2.4.0 - 2.6.19  */
	static irqreturn_t xpci3xxx_do_interrupt(int irq, void * dev_id, struct pt_regs *regs)
#else /* > 2.6.19 */
	static irqreturn_t xpci3xxx_do_interrupt(int irq, void * dev_id)
#endif
	{
	uint32_t dw_InterruptSource = 0;
	uint32_t dw_TimerInterruptSource = 0;
	uint32_t dw_CounterInterruptSource = 0;
	uint32_t dw_WatchdogInterruptSource = 0;
	uint32_t dw_AnalogInputInterruptSource = 0;
	uint8_t  b_TimerHasGenerateInterrupt = 0;
	uint8_t  b_CounterHasGenerateInterrupt = 0;
	uint8_t  b_WatchdogHasGenerateInterrupt = 0;
	uint8_t  b_AnalogInputHasGenerateInterrupt = 0;
    uint32_t dw_ChannelValue[ADDIDATA_MAX_AI + 1];
	unsigned long irqstate;

	// SN: removed LOCK since it seems to be used only in interrupt function
	XPCI3XXX_LOCK(VOID_TO_PCIDEV(dev_id),&irqstate);
	{

		v_xpci3xxx_TimerInterruptFunction	 (VOID_TO_PCIDEV(dev_id),
											   &dw_TimerInterruptSource,
											   &b_TimerHasGenerateInterrupt);

		if (!b_TimerHasGenerateInterrupt)
			dw_TimerInterruptSource = 0;

		/* Only if the borad is not an APCI-3501 */
		if (VOID_TO_PCIDEV(dev_id)->device != xpci3501_BOARD_DEVICE_ID)
		{
			v_xpci3xxx_CounterInterruptFunction(VOID_TO_PCIDEV(dev_id),
												&dw_CounterInterruptSource,
												&b_CounterHasGenerateInterrupt);

			if (!b_CounterHasGenerateInterrupt)
				dw_CounterInterruptSource = 0;
		}

		v_xpci3xxx_WatchdogInterruptFunction  (VOID_TO_PCIDEV(dev_id),
											   &dw_WatchdogInterruptSource,
											   &b_WatchdogHasGenerateInterrupt);

		if (!b_WatchdogHasGenerateInterrupt)
			dw_WatchdogInterruptSource = 0;

		/* Only if the board is not an APCI-3501 */
		if (VOID_TO_PCIDEV(dev_id)->device != xpci3501_BOARD_DEVICE_ID)
		{
			v_xpci3xxx_AnalogInputInterruptFunction (VOID_TO_PCIDEV(dev_id),
													 &dw_AnalogInputInterruptSource,
													 dw_ChannelValue,
													 &b_AnalogInputHasGenerateInterrupt);

			if (!b_AnalogInputHasGenerateInterrupt)
				dw_AnalogInputInterruptSource = 0;
		}

		/* Test if one of the interrupt source occurred */
		if ((!b_TimerHasGenerateInterrupt) && (!b_CounterHasGenerateInterrupt) && (!b_WatchdogHasGenerateInterrupt) && (!b_AnalogInputHasGenerateInterrupt))
		{
			XPCI3XXX_UNLOCK(VOID_TO_PCIDEV(dev_id),irqstate);
			RETURN_NONE;
		}

		/* Save the interrupt source and mask */
		dw_InterruptSource = dw_TimerInterruptSource |
							 dw_CounterInterruptSource |
							 dw_WatchdogInterruptSource |
							 dw_AnalogInputInterruptSource;

		if (dw_InterruptSource != 0)
		{
			/* If the interrupt source is not DMA */
			if ((dw_AnalogInputInterruptSource & 0x1) != 1)
			{
				if (((XPCI3XXX_PRIVDATA(VOID_TO_PCIDEV(dev_id))->dw_EventCounterWrite + 1) % ADDIDATA_MAX_EVENT_COUNTER) != XPCI3XXX_PRIVDATA(VOID_TO_PCIDEV(dev_id))->dw_EventCounterRead)
				{
					XPCI3XXX_PRIVDATA(VOID_TO_PCIDEV(dev_id))->dw_InterruptSource[XPCI3XXX_PRIVDATA(VOID_TO_PCIDEV(dev_id))->dw_EventCounterWrite] = dw_InterruptSource;

					if (dw_InterruptSource & 0x2)
						memcpy(XPCI3XXX_PRIVDATA(VOID_TO_PCIDEV(dev_id))->dw_ChannelValue[XPCI3XXX_PRIVDATA(VOID_TO_PCIDEV(dev_id))->dw_EventCounterWrite],
							   dw_ChannelValue,
							   sizeof(dw_ChannelValue));

					XPCI3XXX_PRIVDATA(VOID_TO_PCIDEV(dev_id))->
					dw_EventCounterWrite = (XPCI3XXX_PRIVDATA(VOID_TO_PCIDEV(dev_id))->dw_EventCounterWrite + 1) % ADDIDATA_MAX_EVENT_COUNTER;
				}
				else
				{
					XPCI3XXX_PRIVDATA(VOID_TO_PCIDEV(dev_id))->
					dw_InterruptSource[XPCI3XXX_PRIVDATA(VOID_TO_PCIDEV(dev_id))->
									   dw_EventCounterWrite] = 0xffffffff;
				}
			}

			/* call registred interrupt callback */
			if (xpci3xxx_private_InterruptCallback)
			{
				(xpci3xxx_private_InterruptCallback) (VOID_TO_PCIDEV(dev_id), dw_InterruptSource);
			}

			/* set asychronous signal notification */
			if (XPCI3XXX_PRIVDATA(VOID_TO_PCIDEV(dev_id))->async_queue)
			{
				kill_fasync( &(XPCI3XXX_PRIVDATA(VOID_TO_PCIDEV(dev_id))->async_queue), SIGIO, POLL_IN);
			}

			/* wake up processes waiting for events */
			wake_up_interruptible (&XPCI3XXX_PRIVDATA(VOID_TO_PCIDEV(dev_id))->wq);
		}
	}
	XPCI3XXX_UNLOCK(VOID_TO_PCIDEV(dev_id),irqstate);

	RETURN_HANDLED;
}

/**
 *
 * Register the interrupt
 *
 * @param[in] pdev       : PCI Device struct         \n
 *
 *
 * @return  0: OK                                                  \n
 *                      <>0: Error                                             \n
 */
int xpci3xxx_register_interrupt(struct pci_dev * pdev)
{
	unsigned long tmp = 0;

	/* Enable the interrupt on the PCI-Express controller */
	if ((pdev->device == xpcie3121_16_8_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3121_16_4_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3121_8_8_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3121_8_4_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3121_16_8C_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3121_16_4C_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3121_8_8C_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3121_8_4C_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3521_8_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3521_4_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3521_8C_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3521_4C_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3021_16_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3021_8_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3021_4_BOARD_DEVICE_ID))
	{
		tmp = inl(GET_BAR1(pdev) + 0x68);
		outl((tmp | (1<<11) | (1 << 8)), GET_BAR1(pdev) + 0x68);
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
	if ( request_irq( pdev->irq, xpci3xxx_do_interrupt, SA_SHIRQ, __DRIVER_NAME, pdev) )
#else
	if ( request_irq( pdev->irq, xpci3xxx_do_interrupt, IRQF_SHARED, __DRIVER_NAME, pdev) )
#endif
	{
		printk(KERN_ERR "%s: can't register interrupt handler for board %s\n", __DRIVER_NAME,pci_name(pdev));
		return -EBUSY;
	}

	else
	{
        printk(KERN_INFO "%s: interrupt handler of board %s registered\n",__DRIVER_NAME,pci_name(pdev));

		/* set internal state information that interrupt are enabled */
		XPCI3XXX_PRIVDATA(pdev)->b_InterruptInstalled = ADDIDATA_ENABLE;

		XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterWrite = 0;

		XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterRead = 0;

		memset (&XPCI3XXX_PRIVDATA(pdev)->dw_InterruptSource, 0, sizeof (XPCI3XXX_PRIVDATA(pdev)->dw_InterruptSource));

		memset (&XPCI3XXX_PRIVDATA(pdev)->dw_ChannelValue, 0, sizeof (XPCI3XXX_PRIVDATA(pdev)->dw_ChannelValue));

		memset (&XPCI3XXX_PRIVDATA(pdev)->dw_SequenceArray, 0, sizeof (XPCI3XXX_PRIVDATA(pdev)->dw_SequenceArray));
	}

	return 0;
}

/**
 *
 * Deregister the interrupt
 *
 * @param[in] pdev       : PCI Device struct           \n
 *
 *
 * @return  0: OK                                                    \n
 *                      <>0: Error                                               \n
 */
int xpci3xxx_deregister_interrupt(struct pci_dev * pdev)
{
	if (XPCI3XXX_PRIVDATA(pdev)->b_InterruptInstalled != ADDIDATA_ENABLE )
	{
		return -EINVAL;
	}

    free_irq( pdev->irq , pdev);

	printk("%s: interrupt handler of board %s deregistered\n",__DRIVER_NAME,pci_name(pdev));

	/* set internal state information that interrupt are enabled */
	XPCI3XXX_PRIVDATA(pdev)->b_InterruptInstalled = ADDIDATA_DISABLE;

	return 0;
}

int do_CMD_xpci3xxx_TestInterrupt(struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{

	uint32_t dw_ArgTable[1 + ADDIDATA_MAX_AI + 1 + ADDIDATA_MAX_INTERRUPT_ANALOG_INPUT_VALUE];
	memset(dw_ArgTable,0,sizeof(dw_ArgTable));

	/* the following doesn't need interrupt locking because:
	 * 1) APCI1500_PRIVDATA(pdev)->wq has already a sping_lock implemented
	 * 2) GET_EVENT_CPTW() and GET_EVENT_CPTR() implement atomic type
	 *  */
	if (wait_event_interruptible (XPCI3XXX_PRIVDATA(pdev)->wq, (XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterRead != XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterWrite)))
		return -ERESTARTSYS;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			if (XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterRead != XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterWrite)
			{
				dw_ArgTable[0] = XPCI3XXX_PRIVDATA(pdev)->dw_InterruptSource[XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterRead];

				// Test if analog input without DMA
				if (dw_ArgTable[0] == 0x2)
					memcpy(&dw_ArgTable[1], XPCI3XXX_PRIVDATA(pdev)->dw_ChannelValue[XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterRead], (ADDIDATA_MAX_AI + 1) * sizeof(uint32_t));

				// Test if analog input with DMA
				if (dw_ArgTable[0] == 0x1)
					memcpy(&dw_ArgTable[1 + ADDIDATA_MAX_AI + 1], XPCI3XXX_PRIVDATA(pdev)->dw_SequenceArray[XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterRead], ADDIDATA_MAX_INTERRUPT_ANALOG_INPUT_VALUE * sizeof(uint32_t));

				XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterRead = (XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterRead + 1) % ADDIDATA_MAX_EVENT_COUNTER;
			}
			else
			{
				dw_ArgTable[0] = 0xfffffffe;
			}
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

    /* Transfers the value to the user */
	if ( copy_to_user( (uint32_t __user *) arg , dw_ArgTable, sizeof(dw_ArgTable) ) )
    	return -EFAULT;

	return 0;
}


//-------------------------------------------------------------------------------

/** Get interrupt value.
 *
 * @param [out]	pdev	The handle of the board that generates the interrupt.
 * @param [out]	mask	The board minor number.<br>
 * 						If 0xfffffffe, fifo is empty.
 * @param [out]	value	Number of buffer and number of value for each buffer, buffer address.
 * @param [out]	shm		A pointer on the shared memory.
 *
 * @retval	0	No error.
 */
int i_xpci3xxx_TestInterrupt(struct pci_dev * pdev, uint32_t *mask, uint32_t *value, uint16_t **shm)
{
	*value = 0;

	if (shm != NULL)
		*shm = NULL;

	if (XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterRead != XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterWrite)
	{
		value[0] = XPCI3XXX_PRIVDATA(pdev)->dw_InterruptSource[XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterRead];

		/* Test if analog input without DMA */
		if (value[0] == 0x2)
			memcpy(&value[1], XPCI3XXX_PRIVDATA(pdev)->dw_ChannelValue[XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterRead], (ADDIDATA_MAX_AI + 1) * sizeof(uint32_t));
		else
		{
			/* Test if analog input with DMA */
			if (value[0] == 0x1)
			{
				memcpy(&value[1 + ADDIDATA_MAX_AI + 1],
					   XPCI3XXX_PRIVDATA(pdev)->dw_SequenceArray[XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterRead],
					   ADDIDATA_MAX_INTERRUPT_ANALOG_INPUT_VALUE * sizeof(uint32_t));

				/* Return a pointer on the shared memory */
				*shm = (uint16_t *) XPCI3XXX_PRIVDATA(pdev)->kmalloc_area;
			}
		}

		XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterRead = (XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterRead + 1) % ADDIDATA_MAX_EVENT_COUNTER;
	}
	else /* nothing in FIFO */
	{
		/* FIFO empty */
		value[0] = 0xfffffffe;
	}

	/* Return the board minor number */
	*mask = XPCI3XXX_PRIVDATA(pdev)->i_MinorNumber;

	return 0;
}

//-------------------------------------------------------------------------------


