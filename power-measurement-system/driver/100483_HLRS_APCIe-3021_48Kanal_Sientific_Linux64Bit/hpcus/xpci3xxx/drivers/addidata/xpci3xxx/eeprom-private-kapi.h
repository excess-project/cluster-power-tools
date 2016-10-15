/** @file eeprom-private-kapi.h
 
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

#ifndef EEPROMPRIVATEKAPI_H_
#define EEPROMPRIVATEKAPI_H_

/** Get the xpci-3xxx input channels number from the eeprom.
 *
 * @param [in,out]	pdev	Pointer on the device structure.
 * 
 * @retval	0	No error.
 * @retval	-1	Timeout.
 */	  
int i_xpci3xxx_ReadEepromHeader_NbrOfOutputs (struct pci_dev *pdev);

#endif /*EEPROMPRIVATEKAPI_H_*/
