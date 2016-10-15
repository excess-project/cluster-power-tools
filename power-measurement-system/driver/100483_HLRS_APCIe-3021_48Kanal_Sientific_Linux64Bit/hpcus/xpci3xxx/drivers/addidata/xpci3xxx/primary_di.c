/** @file primary_di.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
*
* @version  $LastChangedRevision$
* @version $LastChangedDate$
*
* Primary functions (hardware access) for the digital input functionality of the board.
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

EXPORT_SYMBOL(i_xpci3xxx_ReadDigitalInputPort);
EXPORT_SYMBOL(i_xpci3xxx_ReadTTLInputPort);

/**
*
* Read the digital input register status from the board
*
* @param[in]  pdev : PCI Device struct              \n
*
* @param[out] pdw_DigitalInputRegister: Digital input register\n
*
* @return    0 : OK                                               \n
*                       1 : The handle parameter of the board is wrong        \n
*/
int i_xpci3xxx_ReadDigitalInputPort(struct pci_dev * pdev, uint32_t* pdw_DigitalInputRegister)
{
	uint32_t dw_BaseAdress = 0;

	if (!pdev) return 1;

	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_DigitalInputOffsetAdress;
	else
		dw_BaseAdress = GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_DigitalInputOffsetAdress;

	INPDW(dw_BaseAdress, pdw_DigitalInputRegister);

	return 0;
}

/**
*
* Read the TTL register status from the board
*
* @param[in]  pdev : PCI Device struct              \n
*
* @param[out] pdw_TTLInputRegister: TTL register              \n
*
* @return    0 : OK                                               \n
*                       1 : The handle parameter of the board is wrong        \n
*/
int i_xpci3xxx_ReadTTLInputPort(struct pci_dev * pdev, uint32_t* pdw_TTLInputRegister)
{
	uint32_t dw_BaseAdress 			= 0;
	uint32_t dw_TTLRegister 	= 0;

	if (!pdev) return 1;

	*pdw_TTLInputRegister = 0;

	dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
	dw_TTLOffsetAdress;

	INPDW(dw_BaseAdress, &dw_TTLRegister);

	*pdw_TTLInputRegister = dw_TTLRegister & 0xff;

	// Test if Bidirectionnal TTL as input configured
	if(XPCI3XXX_PRIVDATA(pdev)->b_TTLConfiguration == 0x0)
	{
		dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
		dw_TTLOffsetAdress + 0x20;

		INPDW(dw_BaseAdress, &dw_TTLRegister);
		*pdw_TTLInputRegister |= (dw_TTLRegister & 0xff) << 0x8;
	}

	return 0;
}

int i_xpci3xxx_InitDigitalInputModuleFilter (struct pci_dev * pdev,
		uint8_t                                    b_FilterFlag,
		uint32_t                                  dw_ReloadValue)
{
	/************************/
	/* Variable declaration */
	/************************/
	uint32_t dw_BaseAdress = 0;
	uint32_t dw_SetMask = 0;
	uint32_t dw_ResetMask = 0;
	uint32_t dw_WriteValueDWord = 0;

	if (!pdev) return 1;

	if ( (b_FilterFlag != ADDIDATA_DISABLE) && (b_FilterFlag != ADDIDATA_ENABLE) ) return 2;

	if (dw_ReloadValue > 0xff) return 3;

	/*********************************/
	/* Get the Filter address offset */
	/*********************************/
	dw_BaseAdress = GET_BAR2(pdev) + XPCI3XXX_PRIVDATA(pdev)->
	dw_DigitalInputOffsetAdress;

	/****************************/
	/* Set the new filter value */
	/****************************/
	dw_SetMask = dw_ReloadValue;
	dw_ResetMask = 0xff;
	INPDW(dw_BaseAdress + 8, &dw_WriteValueDWord);
	dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
	OUTPDW(dw_BaseAdress + 8, dw_WriteValueDWord);

	/*	SET_RESET_VALUE   (b_CallLevel,
	                   ps_HardwareInformation,
	                   i_AddressOffset * 4,
	                   dw_ReloadValue,
	                   (uint32_t) (pow (2, ps_SingleDigitalInputComponent->b_FilterResolution) - 1),
	                   0,
	                   NULL,
	                   0,
	                   0,
	                   NULL);*/


	/*************************/
	/* Enable disable Filter */
	/*************************/
	dw_SetMask = (uint32_t) b_FilterFlag;
	dw_ResetMask = (uint32_t) (~b_FilterFlag & 3);
	INPDW(dw_BaseAdress + 4, &dw_WriteValueDWord);
	dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
	OUTPDW(dw_BaseAdress + 4, dw_WriteValueDWord);

	/*        SET_RESET_VALUE   (b_CallLevel,
                           ps_HardwareInformation,
                           i_AddressOffset * 4,
                           (uint32_t) b_FilterFlag,
                           (uint32_t) (~b_FilterFlag & 3),
                           0,
                           NULL,
                           0,
                           0,
                           NULL);*/


	/* No error occur */
	return 0;

}
