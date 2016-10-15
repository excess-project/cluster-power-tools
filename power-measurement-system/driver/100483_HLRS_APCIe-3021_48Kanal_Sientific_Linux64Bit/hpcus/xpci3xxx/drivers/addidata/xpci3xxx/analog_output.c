/** @file analog_output.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
*
* @version  $LastChangedRevision$
* @version $LastChangedDate$
*
* Kernel functions for the analog output functionality of the board.
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

int do_CMD_xpci3xxx_InitAnalogOutput (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t   b_Channel;
	uint8_t   b_VoltageMode;
	uint8_t b_ArgTable[2];
	int ret;

//	printk("%s\n",__FUNCTION__); // __FILE__,__FUNCTION__,__LINE__

	if ( copy_from_user( b_ArgTable, (uint8_t __user *) arg, sizeof(b_ArgTable) ) )
		return -EFAULT;

	b_Channel = b_ArgTable[0];
	b_VoltageMode = b_ArgTable[1];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_InitAnalogOutput(pdev, b_Channel, b_VoltageMode);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;
}

int do_CMD_xpci3xxx_ReadAnalogOutputBit (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t   b_AnalogOutputConversionFinished;
	int ret;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_ReadAnalogOutputBit(pdev, &b_AnalogOutputConversionFinished);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	if (ret)
		return ret;

	/* Transfers the value to the user */
	if ( copy_to_user( (uint8_t __user *) arg , &b_AnalogOutputConversionFinished, sizeof(b_AnalogOutputConversionFinished) ) )
		return -EFAULT;

	return 0;
}

int do_CMD_xpci3xxx_WriteAnalogOutputValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t   b_Channel;
	uint32_t dw_WriteValue;
	uint32_t dw_ArgTable[2];
	int ret;

	if ( copy_from_user( dw_ArgTable, (uint32_t __user *) arg, sizeof(dw_ArgTable) ) )
		return -EFAULT;

	b_Channel = (uint8_t)dw_ArgTable[0];
	dw_WriteValue = dw_ArgTable[1];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_WriteAnalogOutputValue(pdev, b_Channel, dw_WriteValue);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;
}

