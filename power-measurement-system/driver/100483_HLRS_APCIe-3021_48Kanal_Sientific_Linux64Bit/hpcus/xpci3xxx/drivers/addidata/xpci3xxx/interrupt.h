/** @file interrupt.h
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
* 
* @version  $LastChangedRevision$
* @version $LastChangedDate$
* 
* Primary functions (hardware access) prototype for the interrupt functionality of the board.
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

#ifndef __xpci3xxx_INTERRUPT__
#define __xpci3xxx_INTERRUPT__

/**
 *
 * Interrupt function for the timer.                      
 *
 * @param[in] pdev       : PCI Device struct         \n
 * @param[in] pdw_InterruptSource : 0x1 : Timer 0             \n
 *                                                   0x2 : Timer 1             \n
 *                                                   0x3 : Timer 0 and 1       \n
 *
 *
 * @return  -                                                      \n
 */
void v_xpci3xxx_TimerInterruptFunction 
                             (struct pci_dev *pdev,                        
                               uint32_t* pdw_InterruptSource,                 
                               uint8_t* pb_HasGenerateInterrupt);

/**
 *
 * Interrupt function for the watchdog.                   
 *
 * @param[in] pdev       : PCI Device struct         \n
 * @param[in] pdw_InterruptSource : 0x4 : Watchdog            \n
 *
 *
 * @return  -                                                      \n
 */
void v_xpci3xxx_WatchdogInterruptFunction 
                             (struct pci_dev *pdev,                        
                               uint32_t* pdw_InterruptSource,                 
                               uint8_t* pb_HasGenerateInterrupt);

/**
 *
 * Interrupt function for the counter.                    
 *
 * @param[in] pdev       : PCI Device struct         \n
 * @param[in] pdw_InterruptSource : 0x8 : Counter             \n
 *
 *
 * @return  -                                                      \n
 */
void v_xpci3xxx_CounterInterruptFunction 
                             (struct pci_dev *pdev,                        
                               uint32_t* pdw_InterruptSource,                 
                               uint8_t* pb_HasGenerateInterrupt);

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
                               uint8_t* pb_HasGenerateInterrupt);

//------------------------------------------------------------------------------
/**  */
int
xpci3xxx_SetIntCallback( void(*InterruptCallback) (struct pci_dev * pdev, uint32_t dw_InterruptSource));

//------------------------------------------------------------------------------

int
xpci3xxx_ResetIntCallback(void);

#endif //__xpci3xxx_INTERRUPT__
