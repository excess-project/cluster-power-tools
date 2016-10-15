/** @file eeprom.h
 
Header of the library used to access the EEPROM of the PCI boards.
 
 
   s5920 header file.
 
   @par CREATION  
   @author Krauth Julien
   @date   06.06.05
   
   @par VERSION
   @verbatim
   $LastChangedRevision: 448 $
   $LastChangedDate: 2007-04-24 16:48:51 +0200 (Di, 24 Apr 2007) $
   @endverbatim   
   
   @par COMPILER
   @verbatim
   Compiler	: gcc
   Version	: gcc (GCC) 3.3.6 (Debian 1:3.3.6-5)   
   kernel	: 2.4.26
   @endverbatim  
   
   @par UPDATES
   @verbatim
   Date			:
   Author		: 
   Description	: 
   @endverbatim       
   
   @par UPDATES
   @verbatim
   Date			:
   Author		: 
   Description	: 
   @endverbatim    
    
   @par LICENCE
   @verbatim
   Copyright (C) 2004,2005 ADDI-DATA GmbH for the source code of this module.
  
         ADDI-DATA GmbH
         Dieselstrasse 3
         D-77833 Ottersweier
         Tel: +19(0)7223/9493-0
         Fax: +49(0)7223/9493-92
         http://www.addi-data-com
         info@addi-data.com
 
   This library is free software; you can redistribute it and/or modify it under 
   the terms of the GNU Lesser General Public License as published by the 
   Free Software Foundation; either version 2.1 of the License, 
   or (at your option) any later version.
 
   This library is distributed in the hope that it will be useful, 
   but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
   or FITNESS FOR A PARTICULAR PURPOSE. 
   See the GNU Lesser General Public License for more details.
 
   You should have received a copy of the GNU Lesser General Public License 
   along with this library; if not, write to the Free Software Foundation, 
   Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 
   You also shoud find the complete LGPL in the LGPL.txt file accompanying 
   this source code.
   @endverbatim   
 */ 

#ifndef __s5920_s5933_H__
#define __s5920_s5933_H__

#include <apci.h>

/** Read data.
 * 
 * @param [in]	NbOfWordsToRead			Number of Words to read.
 * @param [in]	PCIBoardEepromAddress	PCI eeprom address.
 * @param [in]	EepromStartAddress		Start reading address.
 * @param [out]	WordRead				Read data.
 * 
 * @retval	0	No error.
 * @retval	-1	Timeout.
 */
int addi_eeprom_s5920_read(int NbOfWordsToRead,
			uint32_t PCIBoardEepromAddress,
			uint16_t EepromStartAddress,
			uint16_t *DataRead);
			
/** Read data.
 * 
 * @param [in]	NbOfWordsToRead			Number of Words to read.
 * @param [in]	PCIBoardEepromAddress	PCI eeprom address.
 * @param [in]	EepromStartAddress		Start reading address.
 * @param [out]	WordRead				Read data.
 * 
 * @retval	0	No error.
 * @retval	-1	Timeout.
 */
int addi_eeprom_s5933_read(int NbOfWordsToRead,
			uint32_t PCIBoardEepromAddress,
			uint16_t EepromStartAddress,
			uint16_t *DataRead);			

#endif // #ifndef __s5920_s5933_H__
