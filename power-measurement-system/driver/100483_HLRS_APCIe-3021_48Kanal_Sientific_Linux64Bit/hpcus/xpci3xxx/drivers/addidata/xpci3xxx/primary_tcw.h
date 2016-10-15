/** @file primary_tcw.h
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
* 
* @version  $LastChangedRevision$
* @version $LastChangedDate$
* 
* Primary functions (hardware access) prototype for the timer, counter and watchdog functionality of the board.
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

#ifndef __xpci3xxx_PRIMARY_TCW__
#define __xpci3xxx_PRIMARY_TCW__

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
                                             uint8_t                       b_DirectionSelection);

/**
 *
 * Start the selected counter                             
 *
 * @param[in]                    dw_BaseAdress: base adress    \n
 *
 *
 * @return  -                                                      \n
 */
void    v_xpci3xxx_TimerCounterWatchdog_Start (uint32_t                    dw_BaseAdress);


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
                                                   uint32_t       dw_BaseAdress);

/**
 *
 * Clear the selected counter                             
 *
 * @param[in]                    dw_BaseAdress: base adress    \n
 *
 *
 * @return  -                                                      \n
 */
void    v_xpci3xxx_TimerCounterWatchdog_Clear (uint32_t                    dw_BaseAdress);

/**
 *
 * Trigger the selected counter                           
 *
 * @param[in]                    dw_BaseAdress: base adress    \n
 *
 *
 * @return  -                                                      \n
 */
void    v_xpci3xxx_TimerCounterWatchdog_Trigger (uint32_t                    dw_BaseAdress);

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
                                                   uint32_t       dw_BaseAdress);
                                                           
/**
 *
 * Stop the selected counter                              
 *
 * @param[in]                    dw_BaseAdress: base adress    \n
 *
 *
 * @return  -                                                      \n
 */
void    v_xpci3xxx_TimerCounterWatchdog_Stop (uint32_t                    dw_BaseAdress);

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
                                               uint32_t       dw_BaseAdress);


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
                                                  uint32_t*                   pdw_CounterValue);

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
                                                   uint8_t*                     pb_SoftwareClearStatus);

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
                                                                 uint8_t                       b_InterruptFlag);

/**
 *
 * Enable/disable the hardwar counter output              
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
                                                                        uint8_t                       b_OutputLevel);
                                                                        
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
                                                                    uint8_t*                     pb_HardwareOutputStatus);

#endif //__xpci3xxx_PRIMARY_TCW__

