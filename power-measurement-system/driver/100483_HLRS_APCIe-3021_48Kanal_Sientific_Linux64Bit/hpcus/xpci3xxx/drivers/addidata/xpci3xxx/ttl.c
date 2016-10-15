/** @file ttl.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
*
* @version  $LastChangedRevision$
* @version $LastChangedDate$
*
* Kernel functions for the ttl functionality of the board.
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


int do_CMD_xpci3xxx_SetTTLPortConfiguration(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t b_TTLPortConfiguration = 0;
	int ret;

    if ( copy_from_user( &b_TTLPortConfiguration, (uint8_t __user *) arg, sizeof(b_TTLPortConfiguration) ) )
         return -EFAULT;

    switch(b_TTLPortConfiguration)
    {
		case 0:
		case 1:
			break;
		default:
			return -EINVAL;
    }

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			if (XPCI3XXX_PRIVDATA(pdev)->b_TTLSupported != ADDIDATA_ENABLE)
			{
				XPCI3XXX_UNLOCK(pdev,irqstate);
				return 2;
			}
			ret = i_xpci3xxx_SetTTLPortConfiguration(pdev, b_TTLPortConfiguration);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}
	return ret;

}
