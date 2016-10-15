/** @file primary_tcw.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
* 
* @version  $LastChangedRevision$
* @version $LastChangedDate$
* 
* Primary functions (hardware access) for the timer, counter and watchdog functionality of the board.
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

/**@def EXPORT_NO_SYMBOLS
 * Function in this file are not exported.
 */
EXPORT_NO_SYMBOLS;

/**
 *
 * Initialise the counter                                 
 *
 * @param[in]                      b_HardwareSelection :        \n
 *                                                           Operating mode    \n
 *                                                           selection         \n
 *                                                           Timer or Counter  \n
 *                                                           or Watchdog       \n
 * @param[in]                    dw_BaseAdress: base adress    \n
 * @param[in]                      b_Mode :  Mode selection     \n
 * @param[in]                      b_CounterTimeUnit  :         \n
 *                                                           Counter unit      \n
 *                                                           0 : ns            \n
 *                                                           1 : ?s            \n
 *                                                           2 : ms            \n
 *                                                           3 : s             \n
 * @param[in]                    dw_ReloadValue      :         \n
 *                                                           Counter reload    \n
 *                                                           value             \n
 * @param[in]                      b_DirectionSelection:        \n
 *                                                           ADDIDATA_UP :     \n
 *                                                           Select the        \n
 *                                                           counter for       \n
 *                                                           up count          \n
 *                                                           ADDI_DATA_DOWN :  \n
 *                                                           Select the counter\n
 *                                                           for down count    \n
 *
 *
 * @return  -                                                      \n
 */
void    v_xpci3xxx_TimerCounterWatchdog_Init (uint8_t                      b_HardwareSelection,
                                                     uint32_t                    dw_BaseAdress,
                                                     uint8_t                      b_Mode,
                                                     uint8_t                      b_CounterTimeUnit,
                                                     uint32_t                    dw_ReloadValue,
                                                     uint8_t                       b_DirectionSelection)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t dw_Command = 0;   // Command register
        uint32_t dw_SetMask = 0;
        uint32_t dw_ResetMask = 0;
        uint32_t dw_WriteValueDWord = 0;
        

		INPDW(dw_BaseAdress + 12, &dw_Command);

		/*dw_Command = READ_BIT_MASK   (b_CallLevel,
										ps_HardwareInformation,
										12,
										0xFFFFFFFFUL,
										&i_ErrorCodeRBM);*/
		  
		/************************/
		/* Set the reload value */
		/************************/
		OUTPDW(dw_BaseAdress + 4, dw_ReloadValue); 
		/*OUTP_WAIT_READY  (b_CallLevel,
										ps_HardwareInformation,
										4,
										dw_ReloadValue,
										0,
										NULL,
										0,
										0);*/
		  
		/**************************************/
		/* Test if used for watchdog or timer */
		/**************************************/
		if ((b_HardwareSelection == ADDIDATA_TIMER) || 
		      (b_HardwareSelection == ADDIDATA_WATCHDOG))
			{
		    /*********************/
		 	/* Set the time unit */
		 	/*********************/
		 	OUTPDW(dw_BaseAdress + 8, (uint32_t) b_CounterTimeUnit); 
		 	/*OUTP_WAIT_READY  (b_CallLevel,
								   ps_HardwareInformation,
								   8,
								   (uint32_t) b_CounterTimeUnit,
								   0,
								   NULL,
								   0,
								   0);*/
		 
			/**************************/
		 	/* Test if used for timer */
		 	/**************************/
		 	if (b_HardwareSelection == ADDIDATA_TIMER)
		    	{
		    	/******************************/
				/* Set the mode :             */
				/* - Disable the hardware     */
				/* - Disable the counter mode */
				/* - Disable the warning      */
				/* - Disable the reset        */
				/* - Enable the timer mode    */
                    /* - Disable the extern clock */
				/* - Set the timer mode       */
				/******************************/
		 		dw_Command = (dw_Command & 0xFFF419E2UL) | (uint32_t) ((b_Mode << 13UL) | 0x10UL);
			    }
			 else
			    {
			    /******************************/
				/* Set the mode :             */
				/* - Disable the hardware     */
				/* - Disable the counter mode */
				/* - Disable the warning      */
				/* - Disable the reset        */
				/* - Disable the timer mode   */
				/******************************/
		        dw_Command = dw_Command & 0xFFF819E2UL;
		     
		    	}
		    }
		else
		    {
		    /******************************/
			/* Set the mode :             */
			/* - Disable the hardware     */
			/* - Disable the counter mode */
			/* - Disable the warning      */
			/* - Disable the reset        */
			/* - Disable the timer mode   */
			/* - Enable the counter mode  */
			/******************************/
		    dw_Command = (dw_Command & 0xFFFC19E2UL) | 0x80000UL | (uint32_t) ((uint32_t) (b_Mode) << 16UL);
			}
		/*******************/
		/* Set the command */
		/*******************/
		OUTPDW(dw_BaseAdress + 12, dw_Command); 
		/*OUTP_WAIT_READY  (b_CallLevel,
							ps_HardwareInformation,
							12,
							dw_Command,
							0,
							NULL,
							0,
							0);*/
		   
		
		if (b_HardwareSelection == ADDIDATA_COUNTER) 
			{
			/*****************************/
			/* Set the Up/Down selection */
			/*****************************/
			dw_SetMask = (uint32_t) ((uint32_t) b_DirectionSelection << 18);
			dw_ResetMask = 0x600UL | (~((uint32_t) ((uint32_t) b_DirectionSelection << 18)) & 0x40000UL);
			INPDW(dw_BaseAdress + 12, &dw_WriteValueDWord);
			dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
			OUTPDW(dw_BaseAdress + 12, dw_WriteValueDWord); 
			
			/*SET_RESET_VALUE (b_CallLevel,
							  ps_HardwareInformation,
							  12,
							  (uint32_t) ((uint32_t) b_DirectionSelection << 18),
							  0x600UL | (~((uint32_t) ((uint32_t) b_DirectionSelection << 18)) & 0x40000UL),
							  0,
							  NULL,
							  0,
							  0,
							  NULL);*/
			}
        }

/**
 *
 * Enable/disable the counter interrupt                   
 *
 * @param[in]                    dw_BaseAdress: base adress    \n
 * @param[in]                       b_InterruptFlag    :        \n
 *                                                           ADDIDATA_DISABLE :\n
 *                                                           No interrupt      \n
 *                                                           generated from    \n
 *                                                           hardware          \n
 *                                                           ADDIDATA_ENABLE : \n
 *                                                           Interrupt         \n
 *                                                           generated from    \n
 *                                                           hardware          \n
 *
 *
 * @return  -                                                      \n
 */
void    v_xpci3xxx_TimerCounterWatchdog_EnableDisableInterrupt   (uint32_t                    dw_BaseAdress,       
																	uint8_t                       b_InterruptFlag)
        {

		uint32_t dw_SetMask = 0;
		uint32_t dw_ResetMask = 0;
		uint32_t dw_WriteValueDWord = 0;
		
		/*******************************/
		/* Set the interrupt selection */
		/*******************************/
		dw_SetMask = (uint32_t) ((uint32_t) b_InterruptFlag << 1);
		dw_ResetMask = 0x600UL | (~((uint32_t) ((uint32_t) b_InterruptFlag << 1)) & 0x2UL);
		INPDW(dw_BaseAdress + 12, &dw_WriteValueDWord);
		dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
		OUTPDW(dw_BaseAdress + 12, dw_WriteValueDWord); 
		
		/*SET_RESET_VALUE (b_CallLevel,
						   ps_HardwareInformation,
						   12,
						   (uint32_t) ((uint32_t) b_InterruptFlag << 1),
						   0x600UL | (~((uint32_t) ((uint32_t) b_InterruptFlag << 1)) & 0x2UL),
						   0,
						   NULL,
						   0,
						   0,
						   NULL);*/
           
        }

/**
 *
 * Start the selected counter                             
 *
 * @param[in]                    dw_BaseAdress: base adress    \n
 *
 *
 * @return  -                                                      \n
 */
void    v_xpci3xxx_TimerCounterWatchdog_Start (uint32_t                    dw_BaseAdress)
        {
        
        uint32_t dw_SetMask = 0;
        uint32_t dw_ResetMask = 0;
        uint32_t dw_WriteValueDWord = 0;

		/**********************/
		/* Start the hardware */
		/**********************/
		dw_SetMask = 1UL;
		dw_ResetMask = 0x600UL;
		INPDW(dw_BaseAdress + 12, &dw_WriteValueDWord);
		dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
		OUTPDW(dw_BaseAdress + 12, dw_WriteValueDWord);
		
		/*SET_RESET_VALUE (b_CallLevel,
						   ps_HardwareInformation,
						   12,
						   1UL,
						   0x600UL,
						   0,
						   NULL,
						   0,
						   0,
						   NULL);*/
           
        }

/**
 *
 * Start all counters                                     
 *
 * @param[in]                      b_HardwareSelection :        \n
 *                                                           Operating mode    \n
 *                                                           selection         \n
 *                                                           Timer or Counter  \n
 *                                                           or Watchdog       \n
 * @param[in]                    dw_BaseAdress: base adress    \n
 *
 *
 * @return  -                                                      \n
 */
void    v_xpci3xxx_TimerCounterWatchdog_StartAll (uint8_t         b_HardwareSelection,
                                                           uint32_t       dw_BaseAdress)
        {

        /************************/
        /* Variable declaration */
        /************************/
        uint32_t dw_Command = 0;   // Command register

		/*****************************/
		/* Test if used for watchdog */
		/*****************************/
		if (b_HardwareSelection == ADDIDATA_WATCHDOG)
			{
		    /***********************/
		 	/* Start all watchdogs */
		 	/***********************/
		    dw_Command = 0x1UL;
		  
		    }
		else
			{
		    /*****************************/
		 	/* Test if used for watchdog */
		 	/*****************************/
		 	if (b_HardwareSelection == ADDIDATA_TIMER)
		    	{
		    	/********************/
				/* Start all timers */
				/********************/
			    dw_Command = 0x8UL;
			 
			    }
			 else
			    {
				/**********************/
				/* Start all counters */
				/**********************/
		        dw_Command = 0x40UL;
		     
		    	}
			}
		/*******************/
		/* Set the command */
		/*******************/
		OUTPDW(dw_BaseAdress + 0, dw_Command); 
		/*OUTP_WAIT_READY  (b_CallLevel,
							ps_HardwareInformation,
							0,
							dw_Command,
							0,
							NULL,
							0,
							0);*/
   

        }

/**
 *
 * Clear the selected counter                             
 *
 * @param[in]                    dw_BaseAdress: base adress    \n
 *
 *
 * @return  -                                                      \n
 */
void    v_xpci3xxx_TimerCounterWatchdog_Clear (uint32_t                    dw_BaseAdress)
        {

        uint32_t dw_SetMask = 0;
        uint32_t dw_ResetMask = 0;
        uint32_t dw_WriteValueDWord = 0;

		/**********************/
		/* Start the hardware */
		/**********************/
		dw_SetMask = 0x400UL;
		dw_ResetMask = 0x600UL;
		INPDW(dw_BaseAdress + 12, &dw_WriteValueDWord);
		dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
		OUTPDW(dw_BaseAdress + 12, dw_WriteValueDWord); 
		
		/*SET_RESET_VALUE (b_CallLevel,
						   ps_HardwareInformation,
						   12,
						   0x400UL,
						   0x600UL,
						   0,
						   NULL,
						   0,
						   0,
						   NULL);*/
           
        }

/**
 *
 * Trigger the selected counter                           
 *
 * @param[in]                    dw_BaseAdress: base adress    \n
 *
 *
 * @return  -                                                      \n
 */
void    v_xpci3xxx_TimerCounterWatchdog_Trigger (uint32_t                    dw_BaseAdress)
        {

        uint32_t dw_SetMask = 0;
        uint32_t dw_ResetMask = 0;
        uint32_t dw_WriteValueDWord = 0;

		/***************************/
		/* Set the trigger command */
		/***************************/
		dw_SetMask = 0x200UL;
		dw_ResetMask = 0x600UL;
		INPDW(dw_BaseAdress + 12, &dw_WriteValueDWord);
		dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
		OUTPDW(dw_BaseAdress + 12, dw_WriteValueDWord); 
		
		/*SET_RESET_VALUE (b_CallLevel,
						   ps_HardwareInformation,
						   12,
						   0x200UL,
						   0x600UL,
						   0,
						   NULL,
						   0,
						   0,
						   NULL);*/
   
        }

/**
 *
 * Trigger all counters                                   
 *
 * @param[in]                      b_HardwareSelection :        \n
 *                                                           Operating mode    \n
 *                                                           selection         \n
 *                                                           Timer or Counter  \n
 *                                                           or Watchdog       \n
 * @param[in]                    dw_BaseAdress: base adress    \n
 *
 *
 * @return  -                                                      \n
 */
void    v_xpci3xxx_TimerCounterWatchdog_TriggerAll (uint8_t         b_HardwareSelection,
                                                           uint32_t       dw_BaseAdress)
        {

        /************************/
        /* Variable declaration */
        /************************/
        uint32_t dw_Command = 0;   // Command register

		/*****************************/
		/* Test if used for watchdog */
		/*****************************/
		if (b_HardwareSelection == ADDIDATA_WATCHDOG)
			{
		    /*************************/
		 	/* Trigger all watchdogs */
		 	/*************************/
		    dw_Command = 0x4UL;
		  
		    }
		else
			{
		    /*****************************/
		 	/* Test if used for watchdog */
		 	/*****************************/
		 	if (b_HardwareSelection == ADDIDATA_TIMER)
		    	{
		    	/**********************/
				/* Trigger all timers */
				/**********************/
			    dw_Command = 0x20UL;
			 
			    }
			 else
			    {
			    /************************/
				/* Trigger all counters */
				/************************/
		        dw_Command = 0x100UL;
		     
		        }
			}
		/*******************/
		/* Set the command */
		/*******************/
		OUTPDW(dw_BaseAdress + 0, dw_Command); 
		/*OUTP_WAIT_READY  (b_CallLevel,
							ps_HardwareInformation,
							0,
							dw_Command,
							0,
							NULL,
							0,
							0);*/
		   

        }

/**
 *
 * Stop the selected counter                              
 *
 * @param[in]                    dw_BaseAdress: base adress    \n
 *
 *
 * @return  -                                                      \n
 */
void    v_xpci3xxx_TimerCounterWatchdog_Stop (uint32_t                    dw_BaseAdress)
        {

        uint32_t dw_SetMask = 0;
        uint32_t dw_ResetMask = 0;
        uint32_t dw_WriteValueDWord = 0;

		/************************/
		/* disable the hardware */
		/************************/
		dw_SetMask = 0x0UL;
		dw_ResetMask = 0x601UL;
		INPDW(dw_BaseAdress + 12, &dw_WriteValueDWord);
		dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
		OUTPDW(dw_BaseAdress + 12, dw_WriteValueDWord); 
		
		/*SET_RESET_VALUE (b_CallLevel,
						   ps_HardwareInformation,
						   12,
						   0x0UL,
						   0x601UL,
						   0,
						   NULL,
						   0,
						   0,
						   NULL);*/

        }

/**
 *
 * Stop all counters                                      
 *
 * @param[in]                      b_HardwareSelection :        \n
 *                                                           Operating mode    \n
 *                                                           selection         \n
 *                                                           Timer or Counter  \n
 *                                                           or Watchdog       \n
 * @param[in]                    dw_BaseAdress: base adress    \n
 *
 *
 * @return  -                                                      \n
 */
void    v_xpci3xxx_TimerCounterWatchdog_StopAll (uint8_t         b_HardwareSelection,
                                                           uint32_t       dw_BaseAdress)
        {

        /************************/
        /* Variable declaration */
        /************************/
        uint32_t dw_Command = 0;   // Command register

		/*****************************/
		/* Test if used for watchdog */
		/*****************************/
		if (b_HardwareSelection == ADDIDATA_WATCHDOG)
			{
		    /**********************/
			/* Stop all watchdogs */
			/**********************/
		    dw_Command = 0x2UL;
		  
		    }
		else
			{
		    /*****************************/
			/* Test if used for watchdog */
			/*****************************/
			if (b_HardwareSelection == ADDIDATA_TIMER)
				{
				/*******************/
				/* Stop all timers */
				/*******************/
				dw_Command = 0x10UL;
			 
			    }
			 else
			    {
			    /*********************/
				/* Stop all counters */
				/*********************/
			    dw_Command = 0x80UL;
			 
			    }
			}
		/*******************/
		/* Set the command */
		/*******************/
		OUTPDW(dw_BaseAdress + 0, dw_Command); 
		/*OUTP_WAIT_READY  (b_CallLevel,
							ps_HardwareInformation,
							0,
							dw_Command,
							0,
							NULL,
							0,
							0);*/

        }

/**
 *
 * Read the counters value                                
 *
 * @param[in]                    dw_BaseAdress: base adress    \n
 *
 * @param[out] pdw_CounterValue : Hardware value               \n
 *
 * @return  -                                                      \n
 */
void    v_xpci3xxx_TimerCounterWatchdog_ReadValue (uint32_t                     dw_BaseAdress,
                                                          uint32_t*                   pdw_CounterValue)
        {
        
		/**********************/
		/* Read counter value */
		/**********************/
		INPDW(dw_BaseAdress + 0, pdw_CounterValue);
		/**pdw_CounterValue = READ_BIT_MASK   (b_CallLevel,
											  ps_HardwareInformation,
											  0,
											  0xFFFFFFFFUL,
											  &i_ErrorCodeRBM);*/
              
        }

/**
 *
 * Read the counters status                               
 *
 * @param[in]                    dw_BaseAdress: base adress    \n
 *
 * @param[out] pb_CounterStatus         : 0 : No Counter        \n
 *                                                           overflow occur    \n
 *                                                       1 : Counter overflow  \n
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
 * @return  -                                                      \n
 */
void    v_xpci3xxx_TimerCounterWatchdog_ReadStatus (uint32_t                     dw_BaseAdress,
                                                           uint8_t*                     pb_CounterStatus,
                                                           uint8_t*                     pb_SoftwareTriggerStatus,
                                                           uint8_t*                     pb_HardwareTriggerStatus,
                                                           uint8_t*                     pb_SoftwareClearStatus)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t dw_Status = 0;   // Status register
        
		/******************/
		/* Get the status */
		/******************/
		INPDW(dw_BaseAdress + 16, &dw_Status);
		dw_Status = dw_Status & 0xf;
		/*dw_Status = READ_BIT_MASK   (b_CallLevel,
									   ps_HardwareInformation,
									   16,
									   0xFUL,
									   &i_ErrorCodeRBM);*/
		  
		/***********************************/
		/* Get the software trigger status */
		/***********************************/
		*pb_SoftwareTriggerStatus = (uint8_t) ((dw_Status >> 1) & 1);
		  
		/***********************************/
		/* Get the hardware trigger status */
		/***********************************/
		*pb_HardwareTriggerStatus = (uint8_t) ((dw_Status >> 2) & 1);
		  
		/*********************************/
		/* Get the software clear status */
		/*********************************/
		*pb_SoftwareClearStatus = (uint8_t) ((dw_Status >> 3) & 1);
		  
		/***************************/
		/* Get the overflow status */
		/***************************/
		*pb_CounterStatus = (uint8_t) ((dw_Status >> 0) & 1);
           
        }


/**
 *
 * Enable/disable the hardware counter output             
 *
 * @param[in]                    dw_BaseAdress: base adress    \n
 * @param[in]                       b_OutputFlag       :        \n
 *                                                           ADDIDATA_DISABLE :\n
 *                                                           Hardware counter  \n
 *                                                           output not used   \n
 *                                                           ADDIDATA_ENABLE  :\n
 *                                                           Hardware counter  \n
 *                                                           output used       \n
 * @param[in]                       b_OutputLevel      :        \n
 *                                                           ADDIDATA_HIGH :   \n
 *                                                           If the counter    \n
 *                                                           counter overflow  \n
 *                                                           the output it's   \n
 *                                                           set to high       \n
 *                                                           ADDIDATA_LOW  :   \n
 *                                                           If the counter    \n
 *                                                           counter overflow  \n
 *                                                           the output it's   \n
 *                                                           set to low        \n
 *
 *
 * @return  -                                                      \n
 */
void    v_xpci3xxx_TimerCounterWatchdog_EnableDisableHardwareOutput (	uint32_t                    dw_BaseAdress,
																	    uint8_t                       b_OutputFlag,
                                                                        uint8_t                       b_OutputLevel)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t dw_Command = 0;   // Command register
        
		/*******************/
		/* Get the command */
		/*******************/
		INPDW(dw_BaseAdress + 12, &dw_Command);
/*		dw_Command = READ_BIT_MASK  (b_CallLevel,
		                                   ps_HardwareInformation,
		                                   12,
		                                   0xFFFFFFFFUL,
		                                   &i_ErrorCodeRBM);*/
              
		/*******************************/
		/* Disable the hardware output */
		/*******************************/
		dw_Command = dw_Command & 0xFFFFE1FFUL;
              
		/************************************/
		/* Test if hardware output eneabled */
		/************************************/
		if (b_OutputFlag == ADDIDATA_ENABLE)
			{
			/*********************************/
			/* Set the hardware output level */
			/*********************************/
			dw_Command = dw_Command | (uint32_t) ((uint32_t) b_OutputLevel << 11);
			}

		/*******************/
		/* Set the command */
		/*******************/
		OUTPDW(dw_BaseAdress + 12, dw_Command); 
		/*OUTP_WAIT_READY  (b_CallLevel,
		                    ps_HardwareInformation,
		                    12,
		                    dw_Command,
		                    0,
		                    NULL,
		                    0,
		                    0);*/
           
        }
/**
 *
 * Return the status from hardware counter output         
 *
 * @param[in]                    dw_BaseAdress: base adress    \n
 *
 * @param[out] pb_HardwareOutputStatus : 0 : Counter output it's\n
 *                                                          not activated      \n
 *                                                          1 : Counter output \n
 *                                                          it's activated     \n
 *
 * @return  -                                                      \n
 */
void    v_xpci3xxx_TimerCounterWatchdog_GetHardwareOutputStatus    (uint32_t                    dw_BaseAdress,
                                                                    uint8_t*                     pb_HardwareOutputStatus)
        {
        /************************/
        /* Variable declaration */
        /************************/
        uint32_t dw_Command = 0;   // Command register

		/*******************/
		/* Get the command */
		/*******************/
		INPDW(dw_BaseAdress + 12, &dw_Command);
		dw_Command = dw_Command & 0x400000UL;
		/*dw_Command = READ_BIT_MASK  (b_CallLevel,
		                               ps_HardwareInformation,
		                               12,
		                               0x400000UL,
		                               &i_ErrorCodeRBM);*/
		  
		/********************************/
		/* Get the extern trigger state */
		/********************************/
		*pb_HardwareOutputStatus = (uint8_t) ((dw_Command >> 22) & 1);
           
        }
