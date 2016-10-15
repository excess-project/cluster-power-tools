/** @file watchdog.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
*
* @version  $LastChangedRevision$
* @version $LastChangedDate$
*
* Kernel functions for the watchdog functionality of the board.
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

#include <linux/module.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#include "xpci3xxx-private.h"

/**@def EXPORT_NO_SYMBOLS
 * Function in this file are not exported.
 */
EXPORT_NO_SYMBOLS;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,27)
#define __user
#endif


int do_CMD_xpci3xxx_InitWatchdog                      (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t dw_ArgTable[3];
	int ret;
	uint8_t   b_WatchdogNumber = 0;
	uint8_t   b_WatchdogTimeUnit = 0;
	uint32_t dw_ReloadValue = 0;

    if ( copy_from_user( dw_ArgTable, (uint32_t __user *)arg, sizeof(dw_ArgTable) ) )
         return -EFAULT;

	b_WatchdogNumber = (uint8_t)dw_ArgTable[0];
	b_WatchdogTimeUnit = (uint8_t)dw_ArgTable[1];
	dw_ReloadValue = dw_ArgTable[2];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_InitWatchdog (pdev, b_WatchdogNumber, b_WatchdogTimeUnit, dw_ReloadValue);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;
}


int do_CMD_xpci3xxx_ReleaseWatchdog                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{

	uint8_t   b_WatchdogNumber = 0;
	int ret;

    if ( copy_from_user( &b_WatchdogNumber, (uint8_t __user *) arg, sizeof(b_WatchdogNumber) ) )
         return -EFAULT;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_ReleaseWatchdog (pdev, b_WatchdogNumber);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;
}

int do_CMD_xpci3xxx_StartWatchdog                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t   b_WatchdogNumber = 0;
	int ret;

    if ( copy_from_user( &b_WatchdogNumber, (uint8_t __user *) arg, sizeof(b_WatchdogNumber) ) )
         return -EFAULT;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_StartWatchdog (pdev, b_WatchdogNumber);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;

}

int do_CMD_xpci3xxx_StartAllWatchdogs                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	int ret;
	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_StartAllWatchdogs (pdev);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;

}


int do_CMD_xpci3xxx_TriggerWatchdog                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{

	uint8_t   b_WatchdogNumber = 0;
	int ret;

    if ( copy_from_user( &b_WatchdogNumber, (uint8_t __user *) arg, sizeof(b_WatchdogNumber) ) )
         return -EFAULT;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_TriggerWatchdog (pdev, b_WatchdogNumber);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;

}

int do_CMD_xpci3xxx_TriggerAllWatchdogs                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	int ret;
	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_TriggerAllWatchdogs (pdev);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;

}

int do_CMD_xpci3xxx_StopWatchdog                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t   b_WatchdogNumber = 0;
	int ret;

	if ( copy_from_user( &b_WatchdogNumber, (uint8_t __user *) arg, sizeof(b_WatchdogNumber) ) )
         return -EFAULT;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_StopWatchdog (pdev, b_WatchdogNumber);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;

}



int do_CMD_xpci3xxx_StopAllWatchdogs                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	int ret;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_StopAllWatchdogs (pdev);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;

}


int do_CMD_xpci3xxx_ReadWatchdogValue  (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{

	uint8_t   b_WatchdogNumber = 0;
	uint32_t dw_WatchdogValue;
	int ret;
	uint32_t dw_ArgTable[2];


	if ( copy_from_user( dw_ArgTable, (uint32_t __user *) arg, sizeof(dw_ArgTable) ) )
         return -EFAULT;

    b_WatchdogNumber = (uint8_t)dw_ArgTable[0];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_ReadWatchdogValue  (pdev, b_WatchdogNumber, &dw_WatchdogValue);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	if (ret)
		return ret;

	dw_ArgTable[1] = dw_WatchdogValue;

      /* Transfers the value to the user */
	if ( copy_to_user( (uint32_t __user *) arg , dw_ArgTable, sizeof(dw_ArgTable) ) )
         return -EFAULT;

    return 0;
}

int do_CMD_xpci3xxx_ReadWatchdogStatus  (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t   b_WatchdogNumber = 0;
    uint8_t b_WatchdogStatus = 0;
    uint8_t b_SoftwareTriggerStatus = 0;
    uint8_t b_HardwareTriggerStatus = 0;

    uint8_t b_ArgTable[4];
	int ret;

	if ( copy_from_user( b_ArgTable, (uint8_t __user *) arg, sizeof(b_ArgTable) ) )
         return -EFAULT;

	b_WatchdogNumber = b_ArgTable[0];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_ReadWatchdogStatus  (pdev,
                                     b_WatchdogNumber,
                                     &b_WatchdogStatus,
                                     &b_SoftwareTriggerStatus,
                                     &b_HardwareTriggerStatus);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	if (ret)
		return ret;

	b_ArgTable[1] = b_WatchdogStatus;
	b_ArgTable[2] = b_SoftwareTriggerStatus;
	b_ArgTable[3] = b_HardwareTriggerStatus;

    /* Transfers the value to the user */
	if ( copy_to_user( (uint8_t __user *) arg , b_ArgTable, sizeof(b_ArgTable) ) )
         return -EFAULT;

	return 0;
}

int do_CMD_xpci3xxx_EnableDisableWatchdogInterrupt          (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
        /************************/
        /* Variable declaration */
        /************************/
	uint8_t   b_WatchdogNumber = 0;
	uint8_t   b_InterruptFlag = 0;
	int ret;
    uint8_t b_ArgTable[2];

	if ( copy_from_user( b_ArgTable, (uint8_t __user *) arg, sizeof(b_ArgTable) ) )
         return -EFAULT;

	b_WatchdogNumber = b_ArgTable[0];
	b_InterruptFlag = b_ArgTable[1];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_EnableDisableWatchdogInterrupt (pdev, b_WatchdogNumber, b_InterruptFlag);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;

}

int do_CMD_xpci3xxx_EnableDisableWatchdogHardwareOutput          (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t   b_WatchdogNumber = 0;
	uint8_t   b_OutputFlag = 0;
	uint8_t   b_OutputLevel = 0;
	int ret;
    uint8_t b_ArgTable[3];

    if ( copy_from_user( b_ArgTable, (uint8_t __user *) arg, sizeof(b_ArgTable) ) )
         return -EFAULT;

	b_WatchdogNumber = b_ArgTable[0];
	b_OutputFlag = b_ArgTable[1];
	b_OutputLevel = b_ArgTable[1];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_EnableDisableWatchdogHardwareOutput (pdev, b_WatchdogNumber, b_OutputFlag, b_OutputLevel);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;

}


int do_CMD_xpci3xxx_GetWatchdogHardwareOutputStatus  (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t   b_WatchdogNumber = 0;
	uint8_t b_HardwareOutputStatus;
	int ret;
	uint8_t b_ArgTable[2];

	if ( copy_from_user( b_ArgTable, (uint8_t __user *) arg, sizeof(b_ArgTable) ) )
         return -EFAULT;

    b_WatchdogNumber = b_ArgTable[0];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_GetWatchdogHardwareOutputStatus  (pdev, b_WatchdogNumber, &b_HardwareOutputStatus);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	if (ret)
		return ret;

	b_ArgTable[1] = b_HardwareOutputStatus;

    /* Transfers the value to the user */
	if ( copy_to_user( (uint8_t __user *) arg , b_ArgTable, sizeof(b_ArgTable) ) )
         return -EFAULT;

    return 0;
}


