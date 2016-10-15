/** @file io_access.c

   Access functions to update the firmware eeprom.

   @par CREATION
   @author Krauth Julien
   @date   05.08.2015

   @par VERSION
   @verbatim
   $LastChangedRevision: $
   $LastChangedDate: $
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

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/** Write 4 byte data on the board.
 *
 * This function can be changed, it is recommended NOT TO USE IT!
 *
 * @ [in] arg :	arg[0] : Offset
 * 				arg[1] : Value
 *
 * @retval 0: No error.
 * @retval -EFAULT : Failed to copy data in user space (incorrect pointer).
 * @retval -EADDRNOTAVAIL : Type of access not available.
 */
int do_CMD_xpci3xxx_OUTPORTDW (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t param[2] = {0};

	if (copy_from_user (param, (uint32_t __user *)arg, sizeof(param)))
		return -EFAULT;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			OUTPDW (GET_BAR2(pdev) + param[0],  param[1]);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return 0;
}

//------------------------------------------------------------------------------

/** Read 4 byte data from the board.
 *
 * This function can be changed, it is recommended NOT TO USE IT!
 *
 * @param[in] arg:	arg[0] : Offset
 * @param[out] arg:	arg[0] : Value
 *
 * @retval 0: No error.
 * @retval -EFAULT : Failed to copy data in user space (incorrect pointer).
 * @retval -EADDRNOTAVAIL : Type of access not available.
 */
int do_CMD_xpci3xxx_INPORTDW (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t value = 0;

	if (copy_from_user (&value, (uint32_t __user *)arg, sizeof(value)))
		return -EFAULT;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			INPDW (GET_BAR2(pdev) + value, &value);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	if ( copy_to_user( (uint32_t __user *)arg , &value, sizeof(uint32_t) ) )
		return -EFAULT;

	return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
