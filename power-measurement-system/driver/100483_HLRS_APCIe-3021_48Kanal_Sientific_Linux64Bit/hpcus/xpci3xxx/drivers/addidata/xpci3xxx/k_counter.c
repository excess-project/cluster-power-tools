/** @file k_counter.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
* 
* @version  $LastChangedRevision$
* @version $LastChangedDate$
* 
* Primary functions (without hardware access) for the counter functionality of the board.
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

EXPORT_SYMBOL(i_xpci3xxx_InitCounter);
EXPORT_SYMBOL(i_xpci3xxx_ReleaseCounter);
EXPORT_SYMBOL(i_xpci3xxx_StartCounter);
EXPORT_SYMBOL(i_xpci3xxx_StartAllCounters);
EXPORT_SYMBOL(i_xpci3xxx_TriggerCounter);
EXPORT_SYMBOL(i_xpci3xxx_TriggerAllCounters);
EXPORT_SYMBOL(i_xpci3xxx_StopCounter);
EXPORT_SYMBOL(i_xpci3xxx_StopAllCounters);
EXPORT_SYMBOL(i_xpci3xxx_ClearCounter);
EXPORT_SYMBOL(i_xpci3xxx_ReadCounterValue);
EXPORT_SYMBOL(i_xpci3xxx_ReadCounterStatus);
EXPORT_SYMBOL(i_xpci3xxx_EnableDisableCounterInterrupt);
EXPORT_SYMBOL(i_xpci3xxx_EnableDisableCounterHardwareOutput);
EXPORT_SYMBOL(i_xpci3xxx_GetCounterHardwareOutputStatus);

/**
 *
 * Initialise the counter                                 
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_CounterNumber  : Selected counter number      \n
 * @param[in]   b_DirectionSelection : ADDIDATA_UP : Select the \n
 *                                                                  counter for\n
 *                                                                  up count   \n
 *                                                    ADDIDATA_DOWN : Select   \n
 *                                                                    the      \n
 *                                                                    counter  \n
 *                                                                    for      \n
 *                                                                    down     \n
 *                                                                    count    \n
 * @param[in]   b_LevelSelection : ADDIDATA_LOW :               \n
 *                                                    Counter count each low   \n
 *                                                    level                    \n
 *                                                ADDIDATA_HIGH :              \n
 *                                                    Counter count each high  \n
 *                                                    level                    \n
 *                                                ADDIDATA_LOW_HIGH :          \n
 *                                                    Counter count each high  \n
 *                                                    and low level            \n
 * @param[in] dw_ReloadValue    : Counter reload value         \n
 *
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 *                       2 : Wrong Counter number                              \n
 *                       3 : Counter as timer already used                     \n
 *                       4 : Counter as watchdog already used                  \n
 *                       5 : Wrong direction selection                         \n
 *                       6 : Wrong level selection                             \n
 *                       7 : Wrong reload value                                \n
 */
int  i_xpci3xxx_InitCounter                      (struct pci_dev *pdev,
                                                         uint8_t   b_CounterNumber,
                                                         uint8_t   b_DirectionSelection,
                                                         uint8_t   b_LevelSelection,
                                                         uint32_t dw_ReloadValue)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_CounterNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the timer number */
		/***********************/
		b_CounterNumberTemp = b_CounterNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter])
            	{
            	if (b_CounterNumberTemp == 0)
            		break;
            	else
            		b_CounterNumberTemp--;
            	}
        	}

        if (b_TCWCounter == ADDIDATA_MAX_TCW) return 2;
      
		/***********************/
		/* Test the configuration */
		/***********************/
       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER))
			return 3;

       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
			return 4;


		/********************************/
		/* Test the direction selection */
		/********************************/
		if ((b_DirectionSelection != ADDIDATA_UP) && (b_DirectionSelection != ADDIDATA_DOWN))
			return 5;

		/********************************/
		/* Test the direction selection */
		/********************************/
		if ((b_LevelSelection != ADDIDATA_LOW) && (b_LevelSelection != ADDIDATA_HIGH) && (b_LevelSelection != ADDIDATA_LOW_HIGH))
			return 6;

		/*******************************/
		/* Test the Counter reload value */
		/*******************************/
		if (dw_ReloadValue >= 65536)
			return 7;


		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
											dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

		/*****************************/
		/* Call the primary function */
		/*****************************/
		v_xpci3xxx_TimerCounterWatchdog_Stop (dw_BaseAdress);
		   
		/*****************************/
		/* Call the primary function */
		/*****************************/
		v_xpci3xxx_TimerCounterWatchdog_Init (ADDIDATA_COUNTER,
                                                    dw_BaseAdress,
                                                    b_LevelSelection,
                                                    0,
                                                    dw_ReloadValue,
                                                    b_DirectionSelection);
                       
		XPCI3XXX_PRIVDATA(pdev)->
		b_TimerCounterWatchdogConfiguration[b_TCWCounter] = ADDIDATA_COUNTER;                    

		return 0;                    
        }


/**
 *
 * Free the counter                                       
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_CounterNumber  : Selected counter number      \n
 *
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 *                       2 : Counter not intialised                            \n
 *                       3 : Wrong counter number                              \n
 *                       4 : Counter as timer already used                     \n
 *                       5 : Counter as Watchdog already used                  \n
 */
int  i_xpci3xxx_ReleaseCounter                    (struct pci_dev *pdev,
													uint8_t   b_CounterNumber)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_CounterNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the watchdog number */
		/***********************/
		b_CounterNumberTemp = b_CounterNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter])
            	{
            	if (b_CounterNumberTemp == 0)
            		{
		        	if (XPCI3XXX_PRIVDATA(pdev)->
		            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER)
	            		break;
	            	else
	            		return 2;
            		}
            	else
            		b_CounterNumberTemp--;
            	}
        	}

        if (b_TCWCounter == ADDIDATA_MAX_TCW) return 3;
      
		/***********************/
		/* Test the configuration */
		/***********************/
       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER))
			return 4;

       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
			return 5;

		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
											dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

		v_xpci3xxx_TimerCounterWatchdog_Stop (dw_BaseAdress);
                       
		v_xpci3xxx_TimerCounterWatchdog_EnableDisableInterrupt (dw_BaseAdress,
																		ADDIDATA_DISABLE);

		XPCI3XXX_PRIVDATA(pdev)->
		b_TimerCounterWatchdogConfiguration[b_TCWCounter] = 0;                    
                    
		return 0;                    
        }

/**
 *
 * Start the selected counter                             
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_CounterNumber  : Selected counter number      \n
 *
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 *                       2 : Counter not intialised                            \n
 *                       3 : Wrong counter number                              \n
 *                       4 : Counter as timer already used                     \n
 *                       5 : Counter as Watchdog already used                  \n
 */
int  i_xpci3xxx_StartCounter                    (struct pci_dev *pdev,
                                                         uint8_t   b_CounterNumber)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_CounterNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the watchdog number */
		/***********************/
		b_CounterNumberTemp = b_CounterNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter])
            	{
            	if (b_CounterNumberTemp == 0)
            		{
		        	if (XPCI3XXX_PRIVDATA(pdev)->
		            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER)
	            		break;
	            	else
	            		return 2;
            		}
            	else
            		b_CounterNumberTemp--;
            	}
        	}

        if (b_TCWCounter == ADDIDATA_MAX_TCW) return 3;
      
		/***********************/
		/* Test the configuration */
		/***********************/
       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER))
			return 4;

       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
			return 5;

		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
											dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

		/*****************************/
		/* Call the primary function */
		/*****************************/
		v_xpci3xxx_TimerCounterWatchdog_Start (dw_BaseAdress);

		return 0;                    
        }


/**
 *
 * Start all counters                                     
 *
 * @param[in] pdev : PCI Device struct               \n
 *
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 */
int  i_xpci3xxx_StartAllCounters                    (struct pci_dev *pdev)
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
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter])
            	{
				if (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER)
					{
					dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
											dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
											
					/*****************************/
					/* Call the primary function */
					/*****************************/
					v_xpci3xxx_TimerCounterWatchdog_StartAll (ADDIDATA_COUNTER, dw_BaseAdress);
					}
            	}
        	}
		
		return 0;                    
        }

/**
 *
 * Trigger the selected counter                           
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_CounterNumber  : Selected counter number      \n
 *
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 *                       2 : Counter not intialised                            \n
 *                       3 : Wrong counter number                              \n
 *                       4 : Counter as timer already used                     \n
 *                       5 : Counter as Watchdog already used                  \n
 */
int  i_xpci3xxx_TriggerCounter                    (struct pci_dev *pdev,
                                                         uint8_t   b_CounterNumber)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_CounterNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the watchdog number */
		/***********************/
		b_CounterNumberTemp = b_CounterNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter])
            	{
            	if (b_CounterNumberTemp == 0)
            		{
		        	if (XPCI3XXX_PRIVDATA(pdev)->
		            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER)
	            		break;
	            	else
	            		return 2;
            		}
            	else
            		b_CounterNumberTemp--;
            	}
        	}

        if (b_TCWCounter == ADDIDATA_MAX_TCW) return 3;
      
		/***********************/
		/* Test the configuration */
		/***********************/
       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER))
			return 4;

       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
			return 5;

		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
											dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

		/*****************************/
		/* Call the primary function */
		/*****************************/
		v_xpci3xxx_TimerCounterWatchdog_Trigger (dw_BaseAdress);
                       
		return 0;                    
        }

/**
 *
 * Trigger all counters                                   
 *
 * @param[in] pdev : PCI Device struct               \n
 *
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 */
int  i_xpci3xxx_TriggerAllCounters                    (struct pci_dev *pdev)
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
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter])
            	{
				if (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER)
					{
					dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
											dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
											
					/*****************************/
					/* Call the primary function */
					/*****************************/
					v_xpci3xxx_TimerCounterWatchdog_TriggerAll (ADDIDATA_COUNTER, dw_BaseAdress);
					}
            	}
        	}
		
		return 0;                    
        }

/**
 *
 * Stop the selected counter                              
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_CounterNumber  : Selected counter number      \n
 *
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 *                       2 : Counter not intialised                            \n
 *                       3 : Wrong counter number                              \n
 *                       4 : Counter as timer already used                     \n
 *                       5 : Counter as Watchdog already used                  \n
 */
int  i_xpci3xxx_StopCounter                    (struct pci_dev *pdev,
                                                         uint8_t   b_CounterNumber)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_CounterNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the watchdog number */
		/***********************/
		b_CounterNumberTemp = b_CounterNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter])
            	{
            	if (b_CounterNumberTemp == 0)
            		{
		        	if (XPCI3XXX_PRIVDATA(pdev)->
		            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER)
	            		break;
	            	else
	            		return 2;
            		}
            	else
            		b_CounterNumberTemp--;
            	}
        	}

        if (b_TCWCounter == ADDIDATA_MAX_TCW) return 3;
      
		/***********************/
		/* Test the configuration */
		/***********************/
       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER))
			return 4;

       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
			return 5;

		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
											dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

		/*****************************/
		/* Call the primary function */
		/*****************************/
		
		v_xpci3xxx_TimerCounterWatchdog_Stop (dw_BaseAdress);
                       
		return 0;                    
        }

/**
 *
 * Stop all counters                                      
 *
 * @param[in] pdev : PCI Device struct               \n
 *
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 */
int  i_xpci3xxx_StopAllCounters                    (struct pci_dev *pdev)
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
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter])
            	{
				if (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER)
					{
					dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
											dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
											
					/*****************************/
					/* Call the primary function */
					/*****************************/
					v_xpci3xxx_TimerCounterWatchdog_StopAll (ADDIDATA_COUNTER, dw_BaseAdress);
					}
            	}
        	}
		
		return 0;                    
        }

/**
 *
 * Clear the selected counter                             
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_CounterNumber  : Selected counter number      \n
 *
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 *                       2 : Counter not intialised                            \n
 *                       3 : Wrong counter number                              \n
 *                       4 : Counter as timer already used                     \n
 *                       5 : Counter as Watchdog already used                  \n
 */
int  i_xpci3xxx_ClearCounter                    (struct pci_dev *pdev,
													 uint8_t   b_CounterNumber)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_CounterNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the watchdog number */
		/***********************/
		b_CounterNumberTemp = b_CounterNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter])
            	{
            	if (b_CounterNumberTemp == 0)
            		{
		        	if (XPCI3XXX_PRIVDATA(pdev)->
		            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER)
	            		break;
	            	else
	            		return 2;
            		}
            	else
            		b_CounterNumberTemp--;
            	}
        	}

        if (b_TCWCounter == ADDIDATA_MAX_TCW) return 3;
      
		/***********************/
		/* Test the configuration */
		/***********************/
       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER))
			return 4;

       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
			return 5;

		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
											dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

		/*****************************/
		/* Call the primary function */
		/*****************************/
		
		v_xpci3xxx_TimerCounterWatchdog_Clear (dw_BaseAdress);
		   
		return 0;                    
        }


/**
 *
 * Read the counters value                                
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_CounterNumber  : Selected counter number      \n
 *
 * @param[out] pdw_CounterValue : Counter value                \n
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 *                       2 : Counter not intialised                            \n
 *                       3 : Wrong counter number                              \n
 *                       4 : Counter as timer already used                     \n
 *                       5 : Counter as Watchdog already used                  \n
 */
int  i_xpci3xxx_ReadCounterValue  (struct pci_dev *pdev,
										uint8_t   b_CounterNumber,
										uint32_t* pdw_CounterValue)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_CounterNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the watchdog number */
		/***********************/
		b_CounterNumberTemp = b_CounterNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter])
            	{
            	if (b_CounterNumberTemp == 0)
            		{
		        	if (XPCI3XXX_PRIVDATA(pdev)->
		            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER)
	            		break;
	            	else
	            		return 2;
            		}
            	else
            		b_CounterNumberTemp--;
            	}
        	}

        if (b_TCWCounter == ADDIDATA_MAX_TCW) return 3;
      
		/***********************/
		/* Test the configuration */
		/***********************/
       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER))
			return 4;

       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
			return 5;

		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
											dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

		/*****************************/
		/* Call the primary function */
		/*****************************/
		v_xpci3xxx_TimerCounterWatchdog_ReadValue (dw_BaseAdress,
		                                                     pdw_CounterValue);
   
		return 0;                    
        }

/**
 *
 * Read the counters status                               
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_CounterNumber  : Selected counter number      \n
 *
 * @param[out] pb_CounterStatus       : 0 : No Counter overflow \n
 *                                                           occur             \n
 *                                                     1 : Counter overflow    \n
 *                                                           occur             \n
 * @param[out] pb_SoftwareTriggerStatus : 0 : No software       \n
 *                                                           trigger occur     \n
 *                                                       1 : Software trigger  \n
 *                                                           occur             \n
 * @param[out] pb_HardwareTriggerStatus : 0 : No hardware       \n
 *                                                           trigger occur     \n
 *                                                       1 : Hardware trigger  \n
 *                                                           occur             \n
 * @param[out] pb_SoftwareClearStatus   : 0 : No software       \n
 *                                                           clear occur       \n
 *                                                       1 : Software clear    \n
 *                                                           occur             \n
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 *                       2 : Counter not intialised                            \n
 *                       3 : Wrong counter number                              \n
 *                       4 : Counter as timer already used                     \n
 *                       5 : Counter as Watchdog already used                  \n
 */
int  i_xpci3xxx_ReadCounterStatus  (struct pci_dev *pdev,
									 uint8_t   b_CounterNumber,
                                     uint8_t* pb_CounterStatus,
                                     uint8_t* pb_SoftwareTriggerStatus,
                                     uint8_t* pb_HardwareTriggerStatus,
                                     uint8_t* pb_SoftwareClearStatus)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_CounterNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the watchdog number */
		/***********************/
		b_CounterNumberTemp = b_CounterNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter])
            	{
            	if (b_CounterNumberTemp == 0)
            		{
		        	if (XPCI3XXX_PRIVDATA(pdev)->
		            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER)
	            		break;
	            	else
	            		return 2;
            		}
            	else
            		b_CounterNumberTemp--;
            	}
        	}

        if (b_TCWCounter == ADDIDATA_MAX_TCW) return 3;
      
		/***********************/
		/* Test the configuration */
		/***********************/
       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER))
			return 4;

       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
			return 5;

		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
											dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

		/*****************************/
		/* Call the primary function */
		/*****************************/
		v_xpci3xxx_TimerCounterWatchdog_ReadStatus (dw_BaseAdress,
		                                                          pb_CounterStatus,
		                                                          pb_SoftwareTriggerStatus,
		                                                          pb_HardwareTriggerStatus,
		                                                          pb_SoftwareClearStatus);
		
		return 0;                    
        }



/**
 *
 * Enable/disable the counter interrupt                   
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_CounterNumber  : Selected counter number      \n
 * @param[in]   b_InterruptFlag : ADDIDATA_DISABLE :            \n
 *                                               No interrupt generated from   \n
 *                                               counter                       \n
 *                                             ADDIDATA_ENABLE :               \n
 *                                             Interrupt generated from counter\n
 *
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 *                       2 : Counter not intialised                            \n
 *                       3 : Wrong counter number                              \n
 *                       4 : Counter as timer already used                     \n
 *                       5 : Counter as Watchdog already used                  \n
 *                       6 : Wrong interrupt flag                              \n
 *                       7 : Interrupt Callback not installed                  \n
 */
int  i_xpci3xxx_EnableDisableCounterInterrupt          (struct pci_dev *pdev,
														uint8_t   b_CounterNumber,
                                                        uint8_t   b_InterruptFlag)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_CounterNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the watchdog number */
		/***********************/
		b_CounterNumberTemp = b_CounterNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter])
            	{
            	if (b_CounterNumberTemp == 0)
            		{
		        	if (XPCI3XXX_PRIVDATA(pdev)->
		            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER)
	            		break;
	            	else
	            		return 2;
            		}
            	else
            		b_CounterNumberTemp--;
            	}
        	}

        if (b_TCWCounter == ADDIDATA_MAX_TCW) return 3;
      
		/***********************/
		/* Test the configuration */
		/***********************/
       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER))
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

		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
											dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

		/*****************************/
		/* Call the primary function */
		/*****************************/
		
		v_xpci3xxx_TimerCounterWatchdog_EnableDisableInterrupt ( dw_BaseAdress,
		                                                                        b_InterruptFlag);
		
		return 0;                    
        }

/**
 *
 * Enable/disable the hardware counter output             
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_CounterNumber   : Selected counter number     \n
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
 *                       2 : Counter not intialised                            \n
 *                       3 : Wrong Counter number                              \n
 *                       4 : Counter as timer already used                     \n
 *                       5 : Counter as watchdog already used                  \n
 *                       6 : Wrong output flag                                 \n
 *                       7 : Wrong output level                                \n
 */
int  i_xpci3xxx_EnableDisableCounterHardwareOutput     (struct pci_dev *pdev,
                                                        uint8_t   b_CounterNumber,
														uint8_t   b_OutputFlag,
                                                        uint8_t   b_OutputLevel)
        {


        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_CounterNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the timer number */
		/***********************/
		b_CounterNumberTemp = b_CounterNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter])
            	{
            	if (b_CounterNumberTemp == 0)
            		{
		        	if (XPCI3XXX_PRIVDATA(pdev)->
		            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER)
	            		break;
	            	else
	            		return 2;
            		}
            	else
            		b_CounterNumberTemp--;
            	}
        	}

        if (b_TCWCounter == ADDIDATA_MAX_TCW) return 3;
      
		/***********************/
		/* Test the configuration */
		/***********************/
       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER))
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

		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
											dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

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
 * Return the status from hardware counter output         
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_CounterNumber  : Selected counter number      \n
 *
 * @param[out] pb_HardwareOutputStatus : 0 : Counter output it's\n
 *                                                          not activated      \n
 *                                                          1 : Counter output \n
 *                                                          it's activated     \n
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 *                       2 : Counter not intialised                            \n
 *                       3 : Wrong Counter number                              \n
 *                       4 : Counter as timer already used                     \n
 *                       5 : Counter as watchdog already used                  \n
 */
int  i_xpci3xxx_GetCounterHardwareOutputStatus  (struct pci_dev *pdev,
			                                    uint8_t     b_CounterNumber,
			                                    uint8_t*    pb_HardwareOutputStatus)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_CounterNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the timer number */
		/***********************/
		b_CounterNumberTemp = b_CounterNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter])
            	{
            	if (b_CounterNumberTemp == 0)
            		{
		        	if (XPCI3XXX_PRIVDATA(pdev)->
		            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER)
	            		break;
	            	else
	            		return 2;
            		}
            	else
            		b_CounterNumberTemp--;
            	}
        	}

        if (b_TCWCounter == ADDIDATA_MAX_TCW) return 3;
      
		/***********************/
		/* Test the configuration */
		/***********************/
       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsTimer[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_TIMER))
			return 4;

       	if ((XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG))
			return 5;

		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
											dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

		/*****************************/
		/* Call the primary function */
		/*****************************/
		v_xpci3xxx_TimerCounterWatchdog_GetHardwareOutputStatus (dw_BaseAdress,
													pb_HardwareOutputStatus);

		return 0;                    
        }
