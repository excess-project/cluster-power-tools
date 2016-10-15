/** @file primary_ttl.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
* 
* @version  $LastChangedRevision$
* @version $LastChangedDate$
* 
* Primary functions (hardware access) for the ttl functionality of the board.
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

EXPORT_SYMBOL(i_xpci3xxx_SetTTLPortConfiguration);

/** 
 *
 * Write the TTL register configuration of the board 
 *
 * @param[in] b_TTLPortConfiguration 	0: TTL will be set as input port (default)\n
 * 										1: TTL will be set as output port 
 *
 * @return  	0 : No error occur\n                                   
 * 				1 : The handle parameter of the board is wrong\n
 *
 */
int i_xpci3xxx_SetTTLPortConfiguration(struct pci_dev * pdev, uint8_t b_TTLPortConfiguration)
{
	uint32_t dw_BaseAdress = 0;

    if (!pdev) return 1;
    
	dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
										dw_TTLOffsetAdress + 0xA0;

    OUTPDW(dw_BaseAdress, (uint32_t)(b_TTLPortConfiguration & 0x1)); 
    
	XPCI3XXX_PRIVDATA(pdev)->b_TTLConfiguration = b_TTLPortConfiguration;

    return 0;
}

