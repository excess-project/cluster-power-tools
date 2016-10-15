/** @file eeprom-kapi.c
 
   Access functions to read header on the eeprom.
 
   @par CREATION  
   @author Krauth Julien
   @date   11.08.06
   
   @par VERSION
   @verbatim
   $LastChangedRevision: 128 $
   $LastChangedDate: 2005-11-18 14:51:33 +0100 (Fri, 18 Nov 2005) $
   @endverbatim   
   
   @par COMPILER
   @verbatim
   Compiler	: gcc
   Version	:   
   kernel	: 
   @endverbatim  
   
   @par UPDATES
   @verbatim
   Date			: 
   Author		: 
   Description	: 
   @endverbatim    
    
   @par LICENCE
   @verbatim
   Copyright (C) 2004,2005  ADDI-DATA GmbH for the source code of this module.
        
        ADDI-DATA GmbH 
        Dieselstrasse 3 
        D-77833 Ottersweier 
        Tel: +19(0)7223/9493-0 
        Fax: +49(0)7223/9493-92 
        http://www.addi-data-com 
        info@addi-data.com 
        
   This program is free software; you can redistribute it and/or modify it under 
   the terms of the GNU General Public License as published by the Free Software 
   Foundation; either version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, 
   but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
   or FITNESS FOR A PARTICULAR PURPOSE. 
   See the GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along with 
   this program; if not, write to the Free Software Foundation, 
   Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

   You shoud also find the complete GPL in the COPYING file 
   accompanying this source code.
   @endverbatim   
 */

#include "xpci3xxx-private.h"

/**@def EXPORT_NO_SYMBOLS
 * Function in this file are not exported.
 */
EXPORT_NO_SYMBOLS;

#ifdef USE_ADDIEEPROM

//------------------------------------------------------------------------------


/** Get the xpci-3xxx serial number from the eeprom.
 *
 * @param [in,out]	pdev	Pointer on the device structure.
 * 
 * @retval	0	No error.
 * @retval	-1	Timeout.
 */	  
int i_xpci3xxx_ReadEepromHeader_SerialNumber (struct pci_dev *pdev)
{
	uint16_t serial_low = 0;
	uint16_t serial_high = 0;
  
		/* Get the board serial number */
		if ((addi_eeprom_s5933_read (1, GET_BAR0(pdev), 0x102, &serial_low)) == -1) return (-1);
		if ((addi_eeprom_s5933_read (1, GET_BAR0(pdev), 0x104, &serial_high)) == -1) return (-1);	
		
		return 0;
}

//------------------------------------------------------------------------------

/** Get the xpci-3xxx output channels number from the eeprom.
 *
 * @param [in,out]	pdev	Pointer on the device structure.
 * 
 * @retval	0	No error.
 * @retval	-1	Timeout.
 */	  
int i_xpci3xxx_ReadEepromHeader_NbrOfOutputs (struct pci_dev *pdev)
{
  uint16_t w_AnalogOutputMainHeaderAddress = 0;
  uint16_t w_AnalogOutputComponentAddress = 0;
  uint16_t w_NumberOfModuls = 0;
  uint16_t w_ModulCounter = 0;
  uint16_t w_FirstHeaderSize = 0;
  uint16_t w_MainHeaderSize = 0;
  uint16_t w_NumberOfOutputs = 0;

	/* Get the Analog output header address */
	if ((addi_eeprom_s5933_read (1, GET_BAR0(pdev), 0x116, &w_AnalogOutputMainHeaderAddress)) == -1) return (-1);
		                    
     /* Compute the real analog output address */
     w_AnalogOutputMainHeaderAddress = w_AnalogOutputMainHeaderAddress + 0x100; 

     /* Get the number of moduls */
     if ((addi_eeprom_s5933_read (1, GET_BAR0(pdev), w_AnalogOutputMainHeaderAddress + 0x06, &w_NumberOfModuls)) == -1) return (-1);
     
     w_NumberOfModuls &= 0x00FF;
                             
     for (w_ModulCounter = 0; w_ModulCounter < w_NumberOfModuls; w_ModulCounter ++)
        {
        /* Compute the component address */
		w_AnalogOutputComponentAddress = w_AnalogOutputMainHeaderAddress + (w_FirstHeaderSize * w_ModulCounter) + 8;         
	   
        /* Read first header size */
     	if ((addi_eeprom_s5933_read (1, GET_BAR0(pdev), w_AnalogOutputComponentAddress, &w_FirstHeaderSize)) == -1) return (-1);
                   
        w_FirstHeaderSize >>= 4;

        /* Read number of outputs */
     	if ((addi_eeprom_s5933_read (1, GET_BAR0(pdev), w_AnalogOutputComponentAddress + 2, &w_NumberOfOutputs)) == -1) return (-1);
        	
        w_NumberOfOutputs >>= 4; 
        
        XPCI3XXX_PRIVDATA(pdev)->b_NumberOfAnalogInput = w_NumberOfOutputs; 	       
        }
        
        return (0);
}

//------------------------------------------------------------------------------

#endif
