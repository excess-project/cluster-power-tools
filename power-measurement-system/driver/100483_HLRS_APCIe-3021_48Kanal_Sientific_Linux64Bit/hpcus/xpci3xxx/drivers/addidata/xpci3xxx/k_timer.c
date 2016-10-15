/** @file k_timer.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
*
* @version  $LastChangedRevision$
* @version $LastChangedDate$
*
* Primary functions (without hardware access) for the timer functionality of the board.
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
#include "primary_tcw.h"

EXPORT_SYMBOL(i_xpci3xxx_InitTimer);
EXPORT_SYMBOL(i_xpci3xxx_ReleaseTimer);
EXPORT_SYMBOL(i_xpci3xxx_StartTimer);
EXPORT_SYMBOL(i_xpci3xxx_StartAllTimers);
EXPORT_SYMBOL(i_xpci3xxx_TriggerTimer);
EXPORT_SYMBOL(i_xpci3xxx_TriggerAllTimers);
EXPORT_SYMBOL(i_xpci3xxx_StopTimer);
EXPORT_SYMBOL(i_xpci3xxx_StopAllTimers);
EXPORT_SYMBOL(i_xpci3xxx_ReadTimerValue);
EXPORT_SYMBOL(i_xpci3xxx_ReadTimerStatus);
EXPORT_SYMBOL(i_xpci3xxx_EnableDisableTimerInterrupt);
EXPORT_SYMBOL(i_xpci3xxx_EnableDisableTimerHardwareOutput);
EXPORT_SYMBOL(i_xpci3xxx_GetTimerHardwareOutputStatus);
EXPORT_SYMBOL(i_xpci3xxx_EnableDisableTimerHardwareTrigger);

/** Enable / Disable the timer hardware trigger
 *
 * @param[in] pdev: PCI Device struct.
 * @param[in] b_TimerNumber: Selected timer index (at that time only timer 0).
 * @param[in] b_HardwareTriggerFlag: 0: Disable / 1: Enable the hardware trigger.
 * @param[in] b_HardwareTriggerLevel:\n
 *            - 1: Trigger on low level.\n
 *            - 2: Trigger on high level.\n
 *            - 3: Trigger on low and high levels.\n
 *
 * @retval 0 : No error occur
 * @retval 1 : The handle parameter of the board is wrong
 * @retval 2 : Timer not initialized
 * @retval 3 : Wrong Timer number
 * @retval 4 : Timer as counter already used
 * @retval 5 : Timer as watchdog already used
 */
int  i_xpci3xxx_EnableDisableTimerHardwareTrigger (struct pci_dev *pdev, uint8_t b_TimerNumber, uint8_t b_HardwareTriggerFlag, uint8_t b_HardwareTriggerLevel)
{
	uint32_t dw_BaseAdress           = 0;
	uint8_t b_TimerNumberTemp 	= 0;
	uint8_t b_TCWCounter		 	= 0;

	if (!pdev)
		return 1;

	/***********************/
	/* Test the timer number */
	/***********************/
	b_TimerNumberTemp = b_TimerNumber;
	for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
	{
		if (XPCI3XXX_PRIVDATA(pdev)->
				b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter])
		{
			if (b_TimerNumberTemp == 0)
			{
				if (XPCI3XXX_PRIVDATA(pdev)->
						b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER)
					break;
				else
					return 2;
			}
			else
				b_TimerNumberTemp--;
		}
	}

	if (b_TCWCounter == ADDIDATA_MAX_TCW)
		return 3;

	/***********************/
	/* Test the configuration */
	/***********************/
	if ((XPCI3XXX_PRIVDATA(pdev)->b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) &&
		(XPCI3XXX_PRIVDATA(pdev)->b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
		return 4;

	if ((XPCI3XXX_PRIVDATA(pdev)->b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) &&
		(XPCI3XXX_PRIVDATA(pdev)->b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
		return 5;

	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
	else
		dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

	{
		uint32_t dw_WriteValueDWord = 0;

		INPDW(dw_BaseAdress + 12, &dw_WriteValueDWord);

		if (b_HardwareTriggerFlag == 0)
			dw_WriteValueDWord &= ~(0x3 << 5);
		else
			dw_WriteValueDWord |= ((b_HardwareTriggerLevel & 0x3) << 5);

		OUTPDW(dw_BaseAdress + 12, dw_WriteValueDWord);
	}

	return 0;
}

/**
*
* Initialise the timer
*
* @param[in] pdev : PCI Device struct               \n
* @param[in]   b_TimerNumber  : Selected timer number          \n
* @param[in]   b_TimerMode    : Timer mode (0 to 5)            \n
* @param[in]   b_TimerTimeUnit: Timer unit 0 : ns              \n
*                                                         1 : ?s              \n
*                                                         2 : ms              \n
*                                                         3 : s               \n
* @param[in] dw_ReloadValue  : Timer reload value             \n
*
*
* @return  0  : No error occur                                    \n
*                       1 : The handle parameter of the board is wrong        \n
*                       2 : Wrong Timer number                                \n
*                       3 : Timer as counter already used                     \n
*                       4 : Timer as watchdog already used                    \n
*                       5 : Wrong Timer mode                                  \n
*                       6 : Wrong Timer time unit                             \n
*                       7 : Wrong reload value                                \n
*/
int  i_xpci3xxx_InitTimer                      (struct pci_dev *pdev,
		uint8_t   b_TimerNumber,
		uint8_t   b_TimerMode,
		uint8_t   b_TimerTimeUnit,
		uint32_t dw_ReloadValue)
{
	/************************/
	/* Variable declaration */
	/************************/
	uint32_t	dw_BaseAdress           = 0;
	uint8_t 	b_TimerNumberTemp 	= 0;
	uint8_t 	b_TCWCounter		 	= 0;

	//	printk("%s:%d\n",__FUNCTION__,__LINE__); // __FILE__,__FUNCTION__,__LINE__

	if (!pdev) return 1;

	/***********************/
	/* Test the timer number */
	/***********************/
	b_TimerNumberTemp = b_TimerNumber;
	for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
	{
		if (XPCI3XXX_PRIVDATA(pdev)->
				b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter])
		{
			if (b_TimerNumberTemp == 0)
				break;
			else
				b_TimerNumberTemp--;
		}
	}

	if (b_TCWCounter == ADDIDATA_MAX_TCW) return 2;

	/***********************/
	/* Test the configuration */
	/***********************/
	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
		return 3;

	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
		return 4;

	/***********************/
	/* Test the timer mode */
	/***********************/
	if ((b_TimerMode != 2) && (b_TimerMode != 3))
		return 5;

	/****************************/
	/* Test the timer time unit */
	/****************************/
	if (((0xE >> b_TimerTimeUnit) & 1) != 1)
		return 6;

	/*******************************/
	/* Test the timer reload value */
	/*******************************/
	if (dw_ReloadValue > 65535)
		return 7;

	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
	else
		dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

	/*****************************/
	/* Call the primary function */
	/*****************************/
	v_xpci3xxx_TimerCounterWatchdog_Stop (dw_BaseAdress);

	/*****************************/
	/* Call the primary function */
	/*****************************/
	v_xpci3xxx_TimerCounterWatchdog_Init (ADDIDATA_TIMER,
			dw_BaseAdress,
			b_TimerMode,
			b_TimerTimeUnit,
			dw_ReloadValue,
			0);

	/* Save the reload value in us for the jitter timer */
	if (b_TCWCounter == 0)
	{
		switch (b_TimerTimeUnit)
		{
			// us
			case 1:
				XPCI3XXX_PRIVDATA(pdev)->dw_ReloadValue = dw_ReloadValue;
			break;

			// ms
			case 2:
				XPCI3XXX_PRIVDATA(pdev)->dw_ReloadValue = (dw_ReloadValue * 1000);
			break;

			// s
			case 3:
				XPCI3XXX_PRIVDATA(pdev)->dw_ReloadValue = (dw_ReloadValue * 1000000);
			break;
		}
	}

	XPCI3XXX_PRIVDATA(pdev)->b_TimerCounterWatchdogConfiguration[b_TCWCounter] = ADDIDATA_TIMER;

	return 0;
}


/**
*
* Free the timer
*
* @param[in] pdev : PCI Device struct               \n
* @param[in]   b_TimerNumber  : Selected timer number          \n
*
*
* @return  0  : No error occur                                    \n
*                       1 : The handle parameter of the board is wrong        \n
*                       2 : Timer not intialised                              \n
*                       3 : Wrong Timer number                                \n
*                       4 : Timer as counter already used                     \n
*                       5 : Timer as watchdog already used                    \n
*/
int  i_xpci3xxx_ReleaseTimer                    (struct pci_dev *pdev,
		uint8_t   b_TimerNumber)
{
	/************************/
	/* Variable declaration */
	/************************/
	uint32_t	dw_BaseAdress           = 0;
	uint8_t 	b_TimerNumberTemp 	= 0;
	uint8_t 	b_TCWCounter		 	= 0;

	if (!pdev) return 1;

	/***********************/
	/* Test the timer number */
	/***********************/
	b_TimerNumberTemp = b_TimerNumber;
	for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
	{
		if (XPCI3XXX_PRIVDATA(pdev)->
				b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter])
		{
			if (b_TimerNumberTemp == 0)
			{
				if (XPCI3XXX_PRIVDATA(pdev)->
						b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER)
					break;
				else
					return 2;
			}
			else
				b_TimerNumberTemp--;
		}
	}

	if (b_TCWCounter == ADDIDATA_MAX_TCW) return 3;

	/***********************/
	/* Test the configuration */
	/***********************/
	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
		return 4;

	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
		return 5;

	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
	else
		dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

	v_xpci3xxx_TimerCounterWatchdog_Stop (dw_BaseAdress);

	v_xpci3xxx_TimerCounterWatchdog_EnableDisableInterrupt (dw_BaseAdress,
			ADDIDATA_DISABLE);

	XPCI3XXX_PRIVDATA(pdev)->
	b_TimerCounterWatchdogConfiguration[b_TCWCounter] = 0;

	return 0;
}


/**
*
* Start the selected timer
*
* @param[in] pdev : PCI Device struct               \n
* @param[in]   b_TimerNumber  : Selected timer number          \n
*
*
* @return  0  : No error occur                                    \n
*                       1 : The handle parameter of the board is wrong        \n
*                       2 : Timer not intialised                              \n
*                       3 : Wrong Timer number                                \n
*                       4 : Timer as counter already used                     \n
*                       5 : Timer as watchdog already used                    \n
*/
int  i_xpci3xxx_StartTimer                    (struct pci_dev *pdev,
		uint8_t   b_TimerNumber)
{
	/************************/
	/* Variable declaration */
	/************************/
	uint32_t	dw_BaseAdress           = 0;
	uint8_t 	b_TimerNumberTemp 	= 0;
	uint8_t 	b_TCWCounter		 	= 0;

	if (!pdev) return 1;

	/***********************/
	/* Test the timer number */
	/***********************/
	b_TimerNumberTemp = b_TimerNumber;
	for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
	{
		if (XPCI3XXX_PRIVDATA(pdev)->
				b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter])
		{
			if (b_TimerNumberTemp == 0)
			{
				if (XPCI3XXX_PRIVDATA(pdev)->
						b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER)
					break;
				else
					return 2;
			}
			else
				b_TimerNumberTemp--;
		}
	}

	if (b_TCWCounter == ADDIDATA_MAX_TCW) return 3;

	/***********************/
	/* Test the configuration */
	/***********************/
	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
		return 4;

	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
		return 5;

	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
	else
		dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

	/*****************************/
	/* Call the primary function */
	/*****************************/
	v_xpci3xxx_TimerCounterWatchdog_Start (dw_BaseAdress);

	return 0;
}

/**
*
* Start all timers
*
* @param[in] pdev : PCI Device struct               \n
*
*
* @return  0  : No error occur                                    \n
*                       1 : The handle parameter of the board is wrong        \n
*/
int  i_xpci3xxx_StartAllTimers                    (struct pci_dev *pdev)
{
	/************************/
	/* Variable declaration */
	/************************/
	uint32_t	dw_BaseAdress           = 0;
	uint8_t 	b_TCWCounter		 	= 0;

	if (!pdev) return 1;

	for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
	{
		if (XPCI3XXX_PRIVDATA(pdev)->
				b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter])
		{
			if (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER)
			{
				if (pdev->device != xpci3501_BOARD_DEVICE_ID)
					dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
				else
					dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

				/*****************************/
				/* Call the primary function */
				/*****************************/
				v_xpci3xxx_TimerCounterWatchdog_StartAll (ADDIDATA_TIMER, dw_BaseAdress);
			}
		}
	}

	return 0;
}


/**
*
* Trigger the selected timer
*
* @param[in] pdev : PCI Device struct               \n
* @param[in]   b_TimerNumber  : Selected timer number          \n
*
*
* @return  0  : No error occur                                    \n
*                       1 : The handle parameter of the board is wrong        \n
*                       2 : Timer not intialised                              \n
*                       3 : Wrong Timer number                                \n
*                       4 : Timer as counter already used                     \n
*                       5 : Timer as watchdog already used                    \n
*/
int  i_xpci3xxx_TriggerTimer                    (struct pci_dev *pdev,
		uint8_t   b_TimerNumber)
{
	/************************/
	/* Variable declaration */
	/************************/
	uint32_t	dw_BaseAdress           = 0;
	uint8_t 	b_TimerNumberTemp 	= 0;
	uint8_t 	b_TCWCounter		 	= 0;

	if (!pdev) return 1;

	/***********************/
	/* Test the timer number */
	/***********************/
	b_TimerNumberTemp = b_TimerNumber;
	for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
	{
		if (XPCI3XXX_PRIVDATA(pdev)->
				b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter])
		{
			if (b_TimerNumberTemp == 0)
			{
				if (XPCI3XXX_PRIVDATA(pdev)->
						b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER)
					break;
				else
					return 2;
			}
			else
				b_TimerNumberTemp--;
		}
	}

	if (b_TCWCounter == ADDIDATA_MAX_TCW) return 3;

	/***********************/
	/* Test the configuration */
	/***********************/
	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
		return 4;

	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
		return 5;

	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
	else
		dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];


	/*****************************/
	/* Call the primary function */
	/*****************************/
	v_xpci3xxx_TimerCounterWatchdog_Trigger (dw_BaseAdress);


	return 0;
}

/**
*
* Trigger all timers
*
* @param[in] pdev : PCI Device struct               \n
*
*
* @return  0  : No error occur                                    \n
*                       1 : The handle parameter of the board is wrong        \n
*/
int  i_xpci3xxx_TriggerAllTimers                    (struct pci_dev *pdev)
{
	/************************/
	/* Variable declaration */
	/************************/
	uint32_t	dw_BaseAdress           = 0;
	uint8_t 	b_TCWCounter		 	= 0;

	if (!pdev) return 1;

	for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
	{
		if (XPCI3XXX_PRIVDATA(pdev)->
				b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter])
		{
			if (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER)
			{
				if (pdev->device != xpci3501_BOARD_DEVICE_ID)
					dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
				else
					dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

				/*****************************/
				/* Call the primary function */
				/*****************************/
				v_xpci3xxx_TimerCounterWatchdog_TriggerAll (ADDIDATA_TIMER, dw_BaseAdress);
			}
		}
	}

	return 0;
}

/**
*
* Stop the selected timer
*
* @param[in] pdev : PCI Device struct               \n
* @param[in]   b_TimerNumber  : Selected timer number          \n
*
*
* @return  0  : No error occur                                    \n
*                       1 : The handle parameter of the board is wrong        \n
*                       2 : Timer not intialised                              \n
*                       3 : Wrong Timer number                                \n
*                       4 : Timer as counter already used                     \n
*                       5 : Timer as watchdog already used                    \n
*/
int  i_xpci3xxx_StopTimer                    (struct pci_dev *pdev,
		uint8_t   b_TimerNumber)
{
	/************************/
	/* Variable declaration */
	/************************/
	uint32_t	dw_BaseAdress           = 0;
	uint8_t 	b_TimerNumberTemp 	= 0;
	uint8_t 	b_TCWCounter		 	= 0;

	if (!pdev) return 1;

	/***********************/
	/* Test the timer number */
	/***********************/
	b_TimerNumberTemp = b_TimerNumber;
	for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
	{
		if (XPCI3XXX_PRIVDATA(pdev)->
				b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter])
		{
			if (b_TimerNumberTemp == 0)
			{
				if (XPCI3XXX_PRIVDATA(pdev)->
						b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER)
					break;
				else
					return 2;
			}
			else
				b_TimerNumberTemp--;
		}
	}

	if (b_TCWCounter == ADDIDATA_MAX_TCW) return 3;

	/***********************/
	/* Test the configuration */
	/***********************/
	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
		return 4;

	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
		return 5;

	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
	else
		dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

	/*****************************/
	/* Call the primary function */
	/*****************************/

	v_xpci3xxx_TimerCounterWatchdog_Stop (dw_BaseAdress);

	return 0;
}


/**
*
* Stop all timers
*
* @param[in] pdev : PCI Device struct               \n
*
*
* @return  0  : No error occur                                    \n
*                       1 : The handle parameter of the board is wrong        \n
*/
int  i_xpci3xxx_StopAllTimers                    (struct pci_dev *pdev)
{
	/************************/
	/* Variable declaration */
	/************************/
	uint32_t	dw_BaseAdress           = 0;
	uint8_t 	b_TCWCounter		 	= 0;

	if (!pdev) return 1;

	for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
	{
		if (XPCI3XXX_PRIVDATA(pdev)->
				b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter])
		{
			if (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER)
			{
				if (pdev->device != xpci3501_BOARD_DEVICE_ID)
					dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
				else
					dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

				/*****************************/
				/* Call the primary function */
				/*****************************/
				v_xpci3xxx_TimerCounterWatchdog_StopAll (ADDIDATA_TIMER, dw_BaseAdress);
			}
		}
	}

	return 0;
}

/**
*
* Read the timers value
*
* @param[in] pdev : PCI Device struct               \n
* @param[in]   b_TimerNumber  : Selected timer number          \n
*
* @param[out] pdw_TimerValue : Timer value                    \n
*
* @return  0  : No error occur                                    \n
*                       1 : The handle parameter of the board is wrong        \n
*                       2 : Timer not intialised                              \n
*                       3 : Wrong Timer number                                \n
*                       4 : Timer as counter already used                     \n
*                       5 : Timer as watchdog already used                    \n
*/
int  i_xpci3xxx_ReadTimerValue  (struct pci_dev *pdev,
		uint8_t     b_TimerNumber,
		uint32_t* pdw_TimerValue)
{
	/************************/
	/* Variable declaration */
	/************************/
	uint32_t	dw_BaseAdress           = 0;
	uint8_t 	b_TimerNumberTemp 	= 0;
	uint8_t 	b_TCWCounter		 	= 0;

	if (!pdev) return 1;

	/***********************/
	/* Test the timer number */
	/***********************/
	b_TimerNumberTemp = b_TimerNumber;
	for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
	{
		if (XPCI3XXX_PRIVDATA(pdev)->
				b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter])
		{
			if (b_TimerNumberTemp == 0)
			{
				if (XPCI3XXX_PRIVDATA(pdev)->
						b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER)
					break;
				else
					return 2;
			}
			else
				b_TimerNumberTemp--;
		}
	}

	if (b_TCWCounter == ADDIDATA_MAX_TCW) return 3;

	/***********************/
	/* Test the configuration */
	/***********************/
	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
		return 4;

	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
		return 5;

	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
	else
		dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

	/*****************************/
	/* Call the primary function */
	/*****************************/
	v_xpci3xxx_TimerCounterWatchdog_ReadValue (dw_BaseAdress,
			pdw_TimerValue);

	return 0;
}

int i_xpci3xxx_FastReadTimerValue (struct pci_dev *pdev, uint32_t* value)
{
	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		INPDW((GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[0]), value);
	else
		INPDW((GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[0]), value);

	return 0;
}

/**
*
* Read the timers status
*
* @param[in] pdev : PCI Device struct               \n
* @param[in]   b_TimerNumber  : Selected timer number          \n
*
* @param[out] pb_TimerStatus           : 0 : No Timer overflow \n
*                                                           occur             \n
*                                                       1 : Timer overflow    \n
*                                                           occur             \n
* @param[out] pb_SoftwareTriggerStatus : 0 : No software       \n
*                                                           trigger occur     \n
*                                                       1 : Software trigger  \n
*                                                           occur             \n
* @param[out] pb_HardwareTriggerStatus : 0 : No hardware       \n
*                                                           trigger occur     \n
*                                                       1 : Hardware trigger  \n
*                                                           occur             \n
*
* @return  0  : No error occur                                    \n
*                       1 : The handle parameter of the board is wrong        \n
*                       2 : Timer not intialised                              \n
*                       3 : Wrong Timer number                                \n
*                       4 : Timer as counter already used                     \n
*                       5 : Timer as watchdog already used                    \n
*/
int  i_xpci3xxx_ReadTimerStatus  (struct pci_dev *pdev,
		uint8_t     b_TimerNumber,
		uint8_t* pb_TimerStatus,
		uint8_t* pb_SoftwareTriggerStatus,
		uint8_t* pb_HardwareTriggerStatus)
{
	/************************/
	/* Variable declaration */
	/************************/
	uint32_t	dw_BaseAdress           = 0;
	uint8_t 	b_TimerNumberTemp 	= 0;
	uint8_t 	b_TCWCounter		 	= 0;
	uint8_t	b_ClearStatus        	= 0;

	if (!pdev) return 1;

	/***********************/
	/* Test the timer number */
	/***********************/
	b_TimerNumberTemp = b_TimerNumber;
	for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
	{
		if (XPCI3XXX_PRIVDATA(pdev)->
				b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter])
		{
			if (b_TimerNumberTemp == 0)
			{
				if (XPCI3XXX_PRIVDATA(pdev)->
						b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER)
					break;
				else
					return 2;
			}
			else
				b_TimerNumberTemp--;
		}
	}

	if (b_TCWCounter == ADDIDATA_MAX_TCW) return 3;

	/***********************/
	/* Test the configuration */
	/***********************/
	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
		return 4;

	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
		return 5;

	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
	else
		dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

	/*****************************/
	/* Call the primary function */
	/*****************************/
	v_xpci3xxx_TimerCounterWatchdog_ReadStatus (dw_BaseAdress,
			pb_TimerStatus,
			pb_SoftwareTriggerStatus,
			pb_HardwareTriggerStatus,
			&b_ClearStatus);

	return 0;
}



/**
*
* Enable/disable the timer interrupt
*
* @param[in] pdev : PCI Device struct               \n
* @param[in]   b_TimerNumber   : Selected timer number         \n
* @param[in]   b_InterruptFlag : ADDIDATA_DISABLE :            \n
*                                               No interrupt generated from   \n
*                                               timer                         \n
*                                               ADDIDATA_ENABLE :             \n
*                                               Interrupt generated from timer\n
*
*
* @return  0  : No error occur                                    \n
*                       1 : The handle parameter of the board is wrong        \n
*                       2 : Timer not intialised                              \n
*                       3 : Wrong Timer number                                \n
*                       4 : Timer as counter already used                     \n
*                       5 : Timer as watchdog already used                    \n
*                       6 : Wrong interrupt flag                              \n
*                       7 : Interrupt Callback not installed                  \n
*/
int  i_xpci3xxx_EnableDisableTimerInterrupt          (struct pci_dev *pdev,
		uint8_t   b_TimerNumber,
		uint8_t   b_InterruptFlag)
{


	/************************/
	/* Variable declaration */
	/************************/
	uint32_t	dw_BaseAdress           = 0;
	uint8_t 	b_TimerNumberTemp 	= 0;
	uint8_t 	b_TCWCounter		 	= 0;

	if (!pdev) return 1;

	/***********************/
	/* Test the timer number */
	/***********************/
	b_TimerNumberTemp = b_TimerNumber;
	for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
	{
		if (XPCI3XXX_PRIVDATA(pdev)->
				b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter])
		{
			if (b_TimerNumberTemp == 0)
			{
				if (XPCI3XXX_PRIVDATA(pdev)->
						b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER)
					break;
				else
					return 2;
			}
			else
				b_TimerNumberTemp--;
		}
	}

	if (b_TCWCounter == ADDIDATA_MAX_TCW) return 3;

	/***********************/
	/* Test the configuration */
	/***********************/
	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
		return 4;

	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
		return 5;

	/***************************/
	/* Test the interrupt flag */
	/***************************/
	if ((b_InterruptFlag != ADDIDATA_DISABLE) && (b_InterruptFlag != ADDIDATA_ENABLE))
		return 6;

	if (b_InterruptFlag == ADDIDATA_ENABLE)
	{
		if (XPCI3XXX_PRIVDATA(pdev)->
				b_InterruptInstalled == 0)
		{
			return 7;
		}
	}

	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
	else
		dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

	/*****************************/
	/* Call the primary function */
	/*****************************/
	v_xpci3xxx_TimerCounterWatchdog_EnableDisableInterrupt ( dw_BaseAdress,
			b_InterruptFlag);

	return 0;
}


/**
*
* Enable/disable the hardware timer output
*
* @param[in] pdev : PCI Device struct               \n
* @param[in]   b_TimerNumber   : Selected timer number         \n
* @param[in]   b_OutputFlag       :    ADDIDATA_DISABLE :      \n
*                                                     Hardware counter        \n
*                                                     output not used         \n
*                                                     ADDIDATA_ENABLE  :      \n
*                                                     Hardware counter        \n
*                                                     output used             \n
* @param[in]   b_OutputLevel      :    ADDIDATA_HIGH :         \n
*                                                     If the counter          \n
*                                                     counter overflow        \n
*                                                     the output it's         \n
*                                                     set to high             \n
*                                                     ADDIDATA_LOW  :         \n
*                                                     If the counter          \n
*                                                     counter overflow        \n
*                                                     the output it's         \n
*                                                     set to low              \n
*
*
* @return  0  : No error occur                                    \n
*                       1 : The handle parameter of the board is wrong        \n
*                       2 : Timer not intialised                              \n
*                       3 : Wrong Timer number                                \n
*                       4 : Timer as counter already used                     \n
*                       5 : Timer as watchdog already used                    \n
*                       6 : Wrong output flag                                 \n
*                       7 : Wrong output level                                \n
*/
int  i_xpci3xxx_EnableDisableTimerHardwareOutput     (struct pci_dev *pdev,
		uint8_t   b_TimerNumber,
		uint8_t   b_OutputFlag,
		uint8_t   b_OutputLevel)
{


	/************************/
	/* Variable declaration */
	/************************/
	uint32_t	dw_BaseAdress           = 0;
	uint8_t 	b_TimerNumberTemp 	= 0;
	uint8_t 	b_TCWCounter		 	= 0;

	if (!pdev) return 1;

	/***********************/
	/* Test the timer number */
	/***********************/
	b_TimerNumberTemp = b_TimerNumber;
	for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
	{
		if (XPCI3XXX_PRIVDATA(pdev)->
				b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter])
		{
			if (b_TimerNumberTemp == 0)
			{
				if (XPCI3XXX_PRIVDATA(pdev)->
						b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER)
					break;
				else
					return 2;
			}
			else
				b_TimerNumberTemp--;
		}
	}

	if (b_TCWCounter == ADDIDATA_MAX_TCW) return 3;

	/***********************/
	/* Test the configuration */
	/***********************/
	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
		return 4;

	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
		return 5;

	/************************/
	/* Test the output flag */
	/************************/
	if ((b_OutputFlag != ADDIDATA_DISABLE) && (b_OutputFlag != ADDIDATA_ENABLE))
		return 6;

	/*************************/
	/* Test the output level */
	/*************************/
	if ((b_OutputLevel != ADDIDATA_HIGH) && (b_OutputLevel != ADDIDATA_LOW))
		return 7;

	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
	else
		dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

	/*****************************/
	/* Call the primary function */
	/*****************************/

	v_xpci3xxx_TimerCounterWatchdog_EnableDisableHardwareOutput ( dw_BaseAdress,
			b_OutputFlag,
			b_OutputLevel);

	return 0;
}

/**
*
* Return the status from hardware timer output
*
* @param[in] pdev : PCI Device struct               \n
* @param[in]   b_TimerNumber  : Selected timer number          \n
*
* @param[out] pb_HardwareOutputStatus : 0 : Counter output it's\n
*                                                          not activated      \n
*                                                          1 : Counter output \n
*                                                          it's activated     \n
*
* @return  0  : No error occur                                    \n
*                       1 : The handle parameter of the board is wrong        \n
*                       2 : Timer not intialised                              \n
*                       3 : Wrong Timer number                                \n
*                       4 : Timer as counter already used                     \n
*                       5 : Timer as watchdog already used                    \n
*/
int  i_xpci3xxx_GetTimerHardwareOutputStatus  (struct pci_dev *pdev,
		uint8_t     b_TimerNumber,
		uint8_t*    pb_HardwareOutputStatus)
{
	/************************/
	/* Variable declaration */
	/************************/
	uint32_t	dw_BaseAdress           = 0;
	uint8_t 	b_TimerNumberTemp 	= 0;
	uint8_t 	b_TCWCounter		 	= 0;

	if (!pdev) return 1;

	/***********************/
	/* Test the timer number */
	/***********************/
	b_TimerNumberTemp = b_TimerNumber;
	for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
	{
		if (XPCI3XXX_PRIVDATA(pdev)->
				b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter])
		{
			if (b_TimerNumberTemp == 0)
			{
				if (XPCI3XXX_PRIVDATA(pdev)->
						b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER)
					break;
				else
					return 2;
			}
			else
				b_TimerNumberTemp--;
		}
	}

	if (b_TCWCounter == ADDIDATA_MAX_TCW) return 3;

	/***********************/
	/* Test the configuration */
	/***********************/
	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
		return 4;

	if ((XPCI3XXX_PRIVDATA(pdev)->
			b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
		return 5;

	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
	else
		dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

	/*****************************/
	/* Call the primary function */
	/*****************************/
	v_xpci3xxx_TimerCounterWatchdog_GetHardwareOutputStatus (dw_BaseAdress,
			pb_HardwareOutputStatus);

	return 0;
}

