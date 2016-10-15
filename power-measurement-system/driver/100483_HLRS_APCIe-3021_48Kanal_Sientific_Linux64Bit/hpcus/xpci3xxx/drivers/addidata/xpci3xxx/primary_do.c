/** @file primary_do.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
* 
* @version  $LastChangedRevision$
* @version $LastChangedDate$
* 
* Primary functions (hardware access) for the digital output functionality of the board.
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

EXPORT_SYMBOL(i_xpci3xxx_SetDigitalOutputPort);
EXPORT_SYMBOL(i_xpci3xxx_SetDigitalOutputRegister);
EXPORT_SYMBOL(i_xpci3xxx_SetTTLOutputPort);
EXPORT_SYMBOL(i_xpci3xxx_GetDigitalOutputPort);
EXPORT_SYMBOL(i_xpci3xxx_GetTTLOutputPort);

/**
 *
 * Write the digital output register of the board         
 *
 * @param[in]  pdev : PCI Device struct              \n
 *
 *
 * @return    0 : OK                                               \n
 *                       1 : The handle parameter of the board is wrong        \n
 */
int i_xpci3xxx_SetDigitalOutputPort(struct pci_dev * pdev)
{
	uint32_t dw_BaseAdress = 0;

    if (!pdev) return 1;
    
    if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_DigitalOutputOffsetAdress;
	else
		dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_DigitalOutputOffsetAdress;

    OUTPDW(dw_BaseAdress, XPCI3XXX_PRIVDATA(pdev)->dw_DigitalOutputRegister & 0xf); 
    
    return 0;
}

/** Set the digital output register value.
 * 
 * The digital output register is a private data. 
 * So this function permits to change the digital output 
 * register value out of the kernel module.       
 *
 * @param[in]  pdev							PCI Device struct.
 * @param[in]  dw_DigitalOutputRegister		Digital output register value.
 *
 * @retval 0	No error
 * @retval 1	The handle parameter of the board is wrong
 */
int i_xpci3xxx_SetDigitalOutputRegister (struct pci_dev * pdev, uint32_t dw_DigitalOutputRegister)
{

    if (!pdev) return 1;
	
	XPCI3XXX_PRIVDATA(pdev)->dw_DigitalOutputRegister = dw_DigitalOutputRegister; 
    
    return 0;
}

/**
 *
 * Write the TTL register of the board                    
 *
 * @param[in]  pdev : PCI Device struct              \n
 *
 *
 * @return    0 : OK                                               \n
 *                       1 : The handle parameter of the board is wrong        \n
 */
int i_xpci3xxx_SetTTLOutputPort(struct pci_dev * pdev)
{
	uint32_t dw_BaseAdress = 0;

    if (!pdev) return 1;
    
	dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
										dw_TTLOffsetAdress + 0x10;

    OUTPDW(dw_BaseAdress, (XPCI3XXX_PRIVDATA(pdev)->dw_DigitalOutputRegister >> 0x10) & 0xff); 
    
	// Test if Bidirectionnal TTL as output configured
	if (XPCI3XXX_PRIVDATA(pdev)->b_TTLConfiguration == 0x1)
		{
		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
											dw_TTLOffsetAdress + 0x30;
	
	    OUTPDW(dw_BaseAdress, (XPCI3XXX_PRIVDATA(pdev)->dw_DigitalOutputRegister >> 0x18) & 0xff); 
		}
    return 0;
}

/**
 *
 * Get the digital output register from the board         
 *
 * @param[in]  pdev : PCI Device struct              \n
 *
 * @param[out] pdw_DigitalOutputRegister: Digital output register  \n
 *
 * @return    0 : OK                                               \n
 *                       1 : The handle parameter of the board is wrong        \n
 */
int i_xpci3xxx_GetDigitalOutputPort(struct pci_dev * pdev, uint32_t* pdw_DigitalOutputRegister)
{
	uint32_t dw_BaseAdress = 0;

	if (!pdev) return 1;

	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_DigitalOutputOffsetAdress;
	else
		dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_DigitalOutputOffsetAdress;

	INPDW(dw_BaseAdress, pdw_DigitalOutputRegister); 
	
	return 0;

}

/**
 *
 * Get the TTL register from the board                    
 *
 * @param[in]  pdev : PCI Device struct              \n
 *
 * @param[out] pdw_TTLOutputRegister: TTL output register      \n
 *
 * @return    0 : OK                                               \n
 *                       1 : The handle parameter of the board is wrong        \n
 */
int i_xpci3xxx_GetTTLOutputPort(struct pci_dev * pdev, uint32_t* pdw_TTLOutputRegister)
{
	uint32_t dw_BaseAdress 	= 0;
	uint32_t dw_TTLRegister 	= 0;

    if (!pdev) return 1;

	*pdw_TTLOutputRegister = 0;

	dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
										dw_TTLOffsetAdress + 0x10;

	INPDW(dw_BaseAdress, &dw_TTLRegister); 
	
	*pdw_TTLOutputRegister |= dw_TTLRegister & 0xff;

	// Test if Bidirectionnal TTL as output configured
	if (XPCI3XXX_PRIVDATA(pdev)->b_TTLConfiguration == 0x1)
		{
		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
											dw_TTLOffsetAdress + 0x30;
	
		INPDW(dw_BaseAdress, &dw_TTLRegister); 
		
		*pdw_TTLOutputRegister |= (dw_TTLRegister << 0x8) & 0xff00;
		}

	return 0;

}
