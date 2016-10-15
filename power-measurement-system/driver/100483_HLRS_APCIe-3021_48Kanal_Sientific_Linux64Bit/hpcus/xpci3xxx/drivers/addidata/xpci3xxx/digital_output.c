/** @file digital_output.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
*
* @version  $LastChangedRevision$
* @version $LastChangedDate$
*
* Kernel functions for the digital output functionality of the board.
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

int do_CMD_xpci3xxx_Set32DigitalOutputsOn(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t dw_DigitalOutputRegister = 0;
	int ret = 0;

	if ( copy_from_user( &dw_DigitalOutputRegister, (uint32_t __user *) arg, sizeof(dw_DigitalOutputRegister) ) )
         return -EFAULT;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			if(XPCI3XXX_PRIVDATA(pdev)->b_DigitalOutputMemoryState == ADDIDATA_ENABLE)
			{
					XPCI3XXX_PRIVDATA(pdev)->dw_DigitalOutputRegister |= dw_DigitalOutputRegister;
			}
			else
			{
					XPCI3XXX_PRIVDATA(pdev)->dw_DigitalOutputRegister = dw_DigitalOutputRegister;
			}

			if (XPCI3XXX_PRIVDATA(pdev)->b_TTLSupported == ADDIDATA_ENABLE)
				ret = i_xpci3xxx_SetTTLOutputPort(pdev);

			if (!ret)
			{
				if(XPCI3XXX_PRIVDATA(pdev)->b_DigitalInOutputSupported == ADDIDATA_ENABLE)
					ret = i_xpci3xxx_SetDigitalOutputPort(pdev);
			}
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;
}

int do_CMD_xpci3xxx_Set32DigitalOutputsOff(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{

	uint32_t dw_DigitalOutputRegister = 0;
	int ret = 0;

	if ( copy_from_user( &dw_DigitalOutputRegister, (uint32_t __user *) arg, sizeof(dw_DigitalOutputRegister) ) )
         return -EFAULT;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{

			if(XPCI3XXX_PRIVDATA(pdev)->b_DigitalOutputMemoryState != ADDIDATA_ENABLE)
			{
				XPCI3XXX_UNLOCK(pdev,irqstate);
				return 2;
			}

			XPCI3XXX_PRIVDATA(pdev)->dw_DigitalOutputRegister &= ~dw_DigitalOutputRegister;

			if (XPCI3XXX_PRIVDATA(pdev)->b_TTLSupported == ADDIDATA_ENABLE)
				ret = i_xpci3xxx_SetTTLOutputPort(pdev);

			if (!ret)
			{
				if(XPCI3XXX_PRIVDATA(pdev)->b_DigitalInOutputSupported == ADDIDATA_ENABLE)
					ret = i_xpci3xxx_SetDigitalOutputPort(pdev);
			}
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;
}

/**
 *
 * Set the digital output memory on
 *
 * @param[in]  pdev : PCI Device struct              \n
 *
 *
 * @return   0 : OK                                                \n
 *                       1 : The handle parameter of the board is wrong        \n
 */
int do_CMD_xpci3xxx_SetDigitalOutputMemoryOn(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			XPCI3XXX_PRIVDATA(pdev)->b_DigitalOutputMemoryState = ADDIDATA_ENABLE;
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}
	return 0;
}

/**
 *
 * Set the digital output memory off
 *
 * @param[in]  pdev : PCI Device struct              \n
 *
 *
 * @return   0 : OK                                                \n
 *                       1 : The handle parameter of the board is wrong        \n
 */
int do_CMD_xpci3xxx_SetDigitalOutputMemoryOff(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			XPCI3XXX_PRIVDATA(pdev)->b_DigitalOutputMemoryState = ADDIDATA_DISABLE;
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}
	return 0;
}


int do_CMD_xpci3xxx_Get32DigitalOutputStatus(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t dw_OutputRegister = 0;
	uint32_t dw_DigitalOutputRegister = 0;
	uint32_t dw_TTLOutputRegister = 0;
	int ret = 0;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			/* Reads the input value from the I/O Base Address */
			if (XPCI3XXX_PRIVDATA(pdev)->b_TTLSupported == ADDIDATA_ENABLE)
				ret = i_xpci3xxx_GetTTLOutputPort(pdev, &dw_TTLOutputRegister);
			if(!ret)
			{
				if(XPCI3XXX_PRIVDATA(pdev)->b_DigitalInOutputSupported == ADDIDATA_ENABLE)
					ret = i_xpci3xxx_GetDigitalOutputPort(pdev, &dw_DigitalOutputRegister);
			}
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	if(ret)
		return ret;

	dw_OutputRegister = ((dw_TTLOutputRegister << 0x10) & 0xffff0000) | (dw_DigitalOutputRegister & 0xf);

	/* Transfers the value to the user */
	if ( copy_to_user( (uint32_t __user *) arg , &dw_OutputRegister, sizeof(dw_OutputRegister) ) )
		return -EFAULT;

	return 0;
}
