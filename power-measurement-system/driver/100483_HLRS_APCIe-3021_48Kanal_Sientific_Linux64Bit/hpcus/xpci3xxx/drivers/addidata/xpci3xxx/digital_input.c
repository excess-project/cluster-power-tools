/** @file digital_input.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
*
* @version  $LastChangedRevision$
* @version $LastChangedDate$
*
* Kernel functions for the digital input functionality of the board.
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

/**@def EXPORT_NO_SYMBOLS
 * Function in this file are not exported.
 */
EXPORT_NO_SYMBOLS;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,27)
#define __user
#endif


int do_CMD_xpci3xxx_Read32DigitalInputs(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t dw_InputRegister = 0;
	uint32_t dw_DigitalInputRegister = 0;
	uint32_t dw_TTLInputRegister = 0;
	int ret = 0;

	/* Reads the input value from the I/O Base Address */

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			if (XPCI3XXX_PRIVDATA(pdev)->b_TTLSupported == ADDIDATA_ENABLE)
			{
				ret = i_xpci3xxx_ReadTTLInputPort(pdev, &dw_TTLInputRegister);

				if (ret)
				{
					XPCI3XXX_UNLOCK(pdev,irqstate);
					return ret;
				}
			}
			if(XPCI3XXX_PRIVDATA(pdev)->b_DigitalInOutputSupported == ADDIDATA_ENABLE)
			{
				ret = i_xpci3xxx_ReadDigitalInputPort(pdev, &dw_DigitalInputRegister);
				if (ret)
				{
					XPCI3XXX_UNLOCK(pdev,irqstate);
					return ret;
				}
			}

		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	dw_InputRegister = ((dw_TTLInputRegister << 0x10) & 0xffff0000) | (dw_DigitalInputRegister & 0xf);

	/* Transfers the value to the user */
	if ( copy_to_user( (uint32_t __user *) arg , &dw_InputRegister, sizeof(dw_InputRegister) ) )
		return -EFAULT;

	return 0;
}

int do_CMD_xpci3xxx_InitDigitalInputModuleFilter (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t   b_FilterFlag = 0;
    uint32_t dw_ReloadValue = 0;
	int ret;
    uint32_t dw_ArgTable[2];

	if ( copy_from_user( dw_ArgTable, (uint32_t __user *) arg, sizeof(dw_ArgTable) ) )
		return -EFAULT;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			if(XPCI3XXX_PRIVDATA(pdev)->b_DigitalInOutputSupported != ADDIDATA_ENABLE)
			{
				XPCI3XXX_UNLOCK(pdev,irqstate);
				return 4;
			}
			b_FilterFlag = (uint8_t) dw_ArgTable[0];
			dw_ReloadValue = dw_ArgTable[1];

			ret = i_xpci3xxx_InitDigitalInputModuleFilter   (pdev, b_FilterFlag, dw_ReloadValue);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;

}

