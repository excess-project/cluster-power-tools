/** @file k_watchdog.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
* 
* @version  $LastChangedRevision$
* @version $LastChangedDate$
* 
* Primary functions (without hardware access) for the watchdog functionality of the board.
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

EXPORT_SYMBOL(i_xpci3xxx_InitWatchdog);
EXPORT_SYMBOL(i_xpci3xxx_ReleaseWatchdog);
EXPORT_SYMBOL(i_xpci3xxx_StartWatchdog);
EXPORT_SYMBOL(i_xpci3xxx_StartAllWatchdogs);
EXPORT_SYMBOL(i_xpci3xxx_TriggerWatchdog);
EXPORT_SYMBOL(i_xpci3xxx_TriggerAllWatchdogs);
EXPORT_SYMBOL(i_xpci3xxx_StopWatchdog);
EXPORT_SYMBOL(i_xpci3xxx_StopAllWatchdogs);
EXPORT_SYMBOL(i_xpci3xxx_ReadWatchdogValue);
EXPORT_SYMBOL(i_xpci3xxx_ReadWatchdogStatus);
EXPORT_SYMBOL(i_xpci3xxx_EnableDisableWatchdogInterrupt);
EXPORT_SYMBOL(i_xpci3xxx_GetWatchdogHardwareOutputStatus);
EXPORT_SYMBOL(i_xpci3xxx_EnableDisableWatchdogHardwareOutput);

/**
 *
 * Initialise the watchdog                                
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_WatchdogNumber  : Selected watchdog number    \n
 * @param[in]   b_WatchdogTimeUnit: Watchdog time unit 0 : ns   \n
 *                                                         1 : micros          \n
 *                                                         2 : ms              \n
 *                                                         3 : s               \n
 * @param[in] dw_ReloadValue  : Watchdog reload value          \n
 *
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 *                       2 : Wrong watchdog number                             \n
 *                       3 : Watchdog as timer already used                    \n
 *                       4 : Watchdog as counter already used                  \n
 *                       5 : Wrong Watchdog time unit                          \n
 *                       6 : Wrong reload value                                \n
 */
int  i_xpci3xxx_InitWatchdog                      (struct pci_dev *pdev,
														uint8_t   b_WatchdogNumber,            
                                                        uint8_t   b_WatchdogTimeUnit,
                                                        uint32_t dw_ReloadValue)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_WatchdogNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the watchdog number */
		/***********************/
		b_WatchdogNumberTemp = b_WatchdogNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter])
            	{
            	if (b_WatchdogNumberTemp == 0)
            		break;
            	else
            		b_WatchdogNumberTemp--;
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
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
			return 4;

		/*******************************/
		/* Test the Watchdog time unit */
		/*******************************/
		if (((0xE >> b_WatchdogTimeUnit) & 1) != 1)
			return 5;

		/**********************************/
		/* Test the Watchdog reload value */
		/**********************************/
		if (dw_ReloadValue > 65535)
			return 6;

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
		v_xpci3xxx_TimerCounterWatchdog_Init (ADDIDATA_WATCHDOG,
		                                                dw_BaseAdress,
		                                                0,
		                                                b_WatchdogTimeUnit,
		                                                dw_ReloadValue,
		                                                0);
                       
		XPCI3XXX_PRIVDATA(pdev)->
		b_TimerCounterWatchdogConfiguration[b_TCWCounter] = ADDIDATA_WATCHDOG;                    

		return 0;                    
        }


/**
 *
 * Free the watchdog                                      
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_WatchdogNumber  : Selected watchdog number    \n
 *
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 *                       2 : Watchdog not intialised                           \n
 *                       3 : Wrong watchdog number                             \n
 *                       4 : Watchdog as timer already used                    \n
 *                       5 : Watchdog as counter already used                  \n
 */
int  i_xpci3xxx_ReleaseWatchdog                    (struct pci_dev *pdev,
                                                         uint8_t   b_WatchdogNumber)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_WatchdogNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the watchdog number */
		/***********************/
		b_WatchdogNumberTemp = b_WatchdogNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter])
            	{
            	if (b_WatchdogNumberTemp == 0)
            		{
		        	if (XPCI3XXX_PRIVDATA(pdev)->
		            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG)
	            		break;
	            	else
	            		return 2;
            		}
            	else
            		b_WatchdogNumberTemp--;
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
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
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
 * Start the selected watchdog                            
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_WatchdogNumber  : Selected watchdog number    \n
 *
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 *                       2 : Watchdog not intialised                           \n
 *                       3 : Wrong watchdog number                             \n
 *                       4 : Watchdog as timer already used                    \n
 *                       5 : Watchdog as counter already used                  \n
 */
int  i_xpci3xxx_StartWatchdog                    (struct pci_dev *pdev,
                                                         uint8_t   b_WatchdogNumber)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_WatchdogNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the watchdog number */
		/***********************/
		b_WatchdogNumberTemp = b_WatchdogNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter])
            	{
            	if (b_WatchdogNumberTemp == 0)
            		{
		        	if (XPCI3XXX_PRIVDATA(pdev)->
		            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG)
	            		break;
	            	else
	            		return 2;
            		}
            	else
            		b_WatchdogNumberTemp--;
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
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
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
 * Start all watchdogs                                    
 *
 * @param[in] pdev : PCI Device struct               \n
 *
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 */
int  i_xpci3xxx_StartAllWatchdogs                    (struct pci_dev *pdev)
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
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter])
            	{
				if (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG)
					{
					if (pdev->device != xpci3501_BOARD_DEVICE_ID)
						dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
					else
						dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

					/*****************************/
					/* Call the primary function */
					/*****************************/
					v_xpci3xxx_TimerCounterWatchdog_StartAll (ADDIDATA_WATCHDOG, dw_BaseAdress);
					}
            	}
        	}
		
		return 0;                    
        }

/**
 *
 * Trigger the selected watchdog                          
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_WatchdogNumber  : Selected watchdog number    \n
 *
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 *                       2 : Watchdog not intialised                           \n
 *                       3 : Wrong watchdog number                             \n
 *                       4 : Watchdog as timer already used                    \n
 *                       5 : Watchdog as counter already used                  \n
 */
int  i_xpci3xxx_TriggerWatchdog                    (struct pci_dev *pdev,
                                                         uint8_t   b_WatchdogNumber)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_WatchdogNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the watchdog number */
		/***********************/
		b_WatchdogNumberTemp = b_WatchdogNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter])
            	{
            	if (b_WatchdogNumberTemp == 0)
            		{
		        	if (XPCI3XXX_PRIVDATA(pdev)->
		            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG)
	            		break;
	            	else
	            		return 2;
            		}
            	else
            		b_WatchdogNumberTemp--;
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
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
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
 * Trigger all watchdogs                                  
 *
 * @param[in] pdev : PCI Device struct               \n
 *
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 */
int  i_xpci3xxx_TriggerAllWatchdogs                    (struct pci_dev *pdev)
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
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter])
            	{
				if (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG)
					{
					if (pdev->device != xpci3501_BOARD_DEVICE_ID)
						dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
					else
						dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
			
					/*****************************/
					/* Call the primary function */
					/*****************************/
					v_xpci3xxx_TimerCounterWatchdog_TriggerAll (ADDIDATA_WATCHDOG, dw_BaseAdress);
					}
            	}
        	}
                       
		return 0;                    
        }

/**
 *
 * Stop the selected watchdog                             
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_WatchdogNumber  : Selected watchdog number    \n
 *
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 *                       2 : Watchdog not intialised                           \n
 *                       3 : Wrong watchdog number                             \n
 *                       4 : Watchdog as timer already used                    \n
 *                       5 : Watchdog as counter already used                  \n
 */
int  i_xpci3xxx_StopWatchdog                    (struct pci_dev *pdev,
                                                         uint8_t   b_WatchdogNumber)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_WatchdogNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the watchdog number */
		/***********************/
		b_WatchdogNumberTemp = b_WatchdogNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter])
            	{
            	if (b_WatchdogNumberTemp == 0)
            		{
		        	if (XPCI3XXX_PRIVDATA(pdev)->
		            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG)
	            		break;
	            	else
	            		return 2;
            		}
            	else
            		b_WatchdogNumberTemp--;
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
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
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
 * Stop all watchdogs                                     
 *
 * @param[in] pdev : PCI Device struct               \n
 *
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 */
int  i_xpci3xxx_StopAllWatchdogs                    (struct pci_dev *pdev)
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
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter])
            	{
				if (XPCI3XXX_PRIVDATA(pdev)->
					b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG)
					{
					if (pdev->device != xpci3501_BOARD_DEVICE_ID)
						dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
					else
						dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
						
					/*****************************/
					/* Call the primary function */
					/*****************************/
					v_xpci3xxx_TimerCounterWatchdog_StopAll (ADDIDATA_WATCHDOG, dw_BaseAdress);
					}
            	}
        	}

		return 0;                    
        }


/**
 *
 * Read the watchdogs value                               
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_WatchdogNumber  : Selected watchdog number    \n
 *
 * @param[out] pdw_WatchdogValue : Watchdog value              \n
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 *                       2 : Watchdog not intialised                           \n
 *                       3 : Wrong watchdog number                             \n
 *                       4 : Watchdog as timer already used                    \n
 *                       5 : Watchdog as counter already used                  \n
 */
int  i_xpci3xxx_ReadWatchdogValue  (struct pci_dev *pdev,
									uint8_t   b_WatchdogNumber,
                                    uint32_t* pdw_WatchdogValue)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_WatchdogNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the watchdog number */
		/***********************/
		b_WatchdogNumberTemp = b_WatchdogNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter])
            	{
            	if (b_WatchdogNumberTemp == 0)
            		{
		        	if (XPCI3XXX_PRIVDATA(pdev)->
		            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG)
	            		break;
	            	else
	            		return 2;
            		}
            	else
            		b_WatchdogNumberTemp--;
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
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
			return 5;

		if (pdev->device != xpci3501_BOARD_DEVICE_ID)
			dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
		else
			dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

		/*****************************/
		/* Call the primary function */
		/*****************************/
		v_xpci3xxx_TimerCounterWatchdog_ReadValue (dw_BaseAdress,
		                                                         pdw_WatchdogValue);
		
		return 0;                    
        }

/**
 *
 * Read the watchdogs status                              
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_WatchdogNumber  : Selected watchdog number    \n
 *
 * @param[out] pb_WatchdogStatus     : 0 : No Watchdog overflow \n
 *                                                           occur             \n
 *                                                    1 : Watchdog overflow    \n
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
 *                       2 : Watchdog not intialised                           \n
 *                       3 : Wrong watchdog number                             \n
 *                       4 : Watchdog as timer already used                    \n
 *                       5 : Watchdog as counter already used                  \n
 */
int  i_xpci3xxx_ReadWatchdogStatus  (struct pci_dev *pdev,
										uint8_t   b_WatchdogNumber,
										uint8_t* pb_WatchdogStatus,
										uint8_t* pb_SoftwareTriggerStatus,
										uint8_t* pb_HardwareTriggerStatus)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_WatchdogNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;
        uint8_t	b_ClearStatus        	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the watchdog number */
		/***********************/
		b_WatchdogNumberTemp = b_WatchdogNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter])
            	{
            	if (b_WatchdogNumberTemp == 0)
            		{
		        	if (XPCI3XXX_PRIVDATA(pdev)->
		            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG)
	            		break;
	            	else
	            		return 2;
            		}
            	else
            		b_WatchdogNumberTemp--;
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
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
			return 5;

		if (pdev->device != xpci3501_BOARD_DEVICE_ID)
			dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];
		else
			dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter];

		/*****************************/
		/* Call the primary function */
		/*****************************/
		v_xpci3xxx_TimerCounterWatchdog_ReadStatus (dw_BaseAdress,
		                                                      pb_WatchdogStatus,
		                                                      pb_SoftwareTriggerStatus,
		                                                      pb_HardwareTriggerStatus,
		                                                      &b_ClearStatus);
           
		return 0;                    
        }

/**
 *
 * Enable/disable the watchdog interrupt                  
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_WatchdogNumber  : Selected watchdog number    \n
 * @param[in]   b_InterruptFlag : ADDIDATA_DISABLE :            \n
 *                                               No interrupt generated from   \n
 *                                               watchdog                      \n
 *                                            ADDIDATA_ENABLE :                \n
 *                                            Interrupt generated from watchdog\n
 *
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 *                       2 : Watchdog not intialised                           \n
 *                       3 : Wrong watchdog number                             \n
 *                       4 : Watchdog as timer already used                    \n
 *                       5 : Watchdog as counter already used                  \n
 *                       6 : Wrong interrupt flag                              \n
 *                       7 : Interrupt Callback not installed                  \n
 */
int  i_xpci3xxx_EnableDisableWatchdogInterrupt          (struct pci_dev *pdev,
															uint8_t   b_WatchdogNumber,
															uint8_t   b_InterruptFlag)
        {


        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_WatchdogNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the watchdog number */
		/***********************/
		b_WatchdogNumberTemp = b_WatchdogNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter])
            	{
            	if (b_WatchdogNumberTemp == 0)
            		{
		        	if (XPCI3XXX_PRIVDATA(pdev)->
		            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG)
	            		break;
	            	else
	            		return 2;
            		}
            	else
            		b_WatchdogNumberTemp--;
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
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
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
 * Enable/disable the hardware watchdog output            
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_WatchdogNumber   : Selected watchdog number   \n
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
 *                       2 : Watchdog not intialised                           \n
 *                       3 : Wrong Watchdog number                             \n
 *                       4 : Watchdog as timer already used                    \n
 *                       5 : Watchdog as counter already used                  \n
 *                       6 : Wrong output flag                                 \n
 *                       7 : Wrong output level                                \n
 */
int  i_xpci3xxx_EnableDisableWatchdogHardwareOutput     (struct pci_dev *pdev,
                                                        uint8_t   b_WatchdogNumber,
														uint8_t   b_OutputFlag,
                                                        uint8_t   b_OutputLevel)
        {


        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_WatchdogNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the timer number */
		/***********************/
		b_WatchdogNumberTemp = b_WatchdogNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter])
            	{
            	if (b_WatchdogNumberTemp == 0)
            		{
		        	if (XPCI3XXX_PRIVDATA(pdev)->
		            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG)
	            		break;
	            	else
	            		return 2;
            		}
            	else
            		b_WatchdogNumberTemp--;
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
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
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
 * Return the status from hardware watchdog output        
 *
 * @param[in] pdev : PCI Device struct               \n
 * @param[in]   b_WatchdogNumber  : Selected watchdog number    \n
 *
 * @param[out] pb_HardwareOutputStatus : 0 : Counter output it's\n
 *                                                          not activated      \n
 *                                                          1 : Counter output \n
 *                                                          it's activated     \n
 *
 * @return  0  : No error occur                                    \n
 *                       1 : The handle parameter of the board is wrong        \n
 *                       2 : Watchdog not intialised                           \n
 *                       3 : Wrong Watchdog number                             \n
 *                       4 : Watchdog as timer already used                    \n
 *                       5 : Watchdog as counter already used                  \n
 */
int  i_xpci3xxx_GetWatchdogHardwareOutputStatus  (struct pci_dev *pdev,
			                                    uint8_t     b_WatchdogNumber,
			                                    uint8_t*    pb_HardwareOutputStatus)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t	dw_BaseAdress           = 0;
        uint8_t 	b_WatchdogNumberTemp 	= 0;
        uint8_t 	b_TCWCounter		 	= 0;

		if (!pdev) return 1;
        
		/***********************/
		/* Test the timer number */
		/***********************/
		b_WatchdogNumberTemp = b_WatchdogNumber;
        for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter ++)
        	{
        	if (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfigurableAsWatchdog[b_TCWCounter])
            	{
            	if (b_WatchdogNumberTemp == 0)
            		{
		        	if (XPCI3XXX_PRIVDATA(pdev)->
		            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_WATCHDOG)
	            		break;
	            	else
	            		return 2;
            		}
            	else
            		b_WatchdogNumberTemp--;
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
            b_TimerCounterWatchdogConfigurableAsCounter[b_TCWCounter]) && (XPCI3XXX_PRIVDATA(pdev)->
            b_TimerCounterWatchdogConfiguration[b_TCWCounter] == ADDIDATA_COUNTER))
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
