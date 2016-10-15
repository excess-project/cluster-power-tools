/** @file counter.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
*
* @version  $LastChangedRevision$
* @version $LastChangedDate$
*
* Kernel functions for the counter functionality of the board.
*
*/

/** @par LICENCE
* @verbatim
Copyright (C) 2004,2005  ADDI-DATA GmbH for the source code and the documentation.

        ADDI-DATA GmbH
        Airpark Business Center
        Airport Boulevard B210
        77836 Rheinmünster
        Germany
        Tel: +49(0)7229/1847-0

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

#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
        #include <linux/config.h>
#else
    #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)
		#include <linux/autoconf.h>
	#else
		#include <generated/autoconf.h>
	#endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,27)
        #define __user
#endif


int do_CMD_xpci3xxx_InitCounter                      (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	/************************/
	/* Variable declaration */
	/************************/
	uint32_t dw_ArgTable[4];

	uint8_t   b_CounterNumber = 0;
	uint8_t   b_DirectionSelection = 0;
	uint8_t   b_LevelSelection = 0;
	uint32_t dw_ReloadValue = 0;
	int ret;

	if ( copy_from_user( dw_ArgTable, (uint32_t __user *)arg, sizeof(dw_ArgTable) ) )
		return -EFAULT;

	b_CounterNumber = (uint8_t)dw_ArgTable[0];
	b_DirectionSelection = (uint8_t)dw_ArgTable[1];
	b_LevelSelection = (uint8_t)dw_ArgTable[2];
	dw_ReloadValue = dw_ArgTable[3];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_InitCounter (pdev, b_CounterNumber, b_DirectionSelection, b_LevelSelection, dw_ReloadValue);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;

}

int do_CMD_xpci3xxx_ReleaseCounter                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	/************************/
	/* Variable declaration */
	/************************/
	uint8_t   b_CounterNumber = 0;
	int ret;

	if ( copy_from_user( &b_CounterNumber, (uint8_t __user *) arg, sizeof(b_CounterNumber) ) )
		return -EFAULT;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_ReleaseCounter (pdev, b_CounterNumber);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}
	return ret;
}

int do_CMD_xpci3xxx_StartCounter                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t   b_CounterNumber = 0;
	int ret;

	if ( copy_from_user( &b_CounterNumber, (uint8_t __user *) arg, sizeof(b_CounterNumber) ) )
		return -EFAULT;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_StartCounter (pdev, b_CounterNumber);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}
	return ret;

}

int do_CMD_xpci3xxx_StartAllCounters                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{

	int ret;
	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_StartAllCounters (pdev);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;

}

int do_CMD_xpci3xxx_TriggerCounter                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t   b_CounterNumber = 0;
	int ret;

	if ( copy_from_user( &b_CounterNumber, (uint8_t __user *) arg, sizeof(b_CounterNumber) ) )
		return -EFAULT;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret =i_xpci3xxx_TriggerCounter (pdev, b_CounterNumber);

		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;

}

int do_CMD_xpci3xxx_TriggerAllCounters                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	int ret;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_TriggerAllCounters (pdev);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;

}

int do_CMD_xpci3xxx_StopCounter                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	int ret;
	uint8_t   b_CounterNumber = 0;

	if ( copy_from_user( &b_CounterNumber, (uint8_t __user *) arg, sizeof(b_CounterNumber) ) )
		return -EFAULT;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_StopCounter (pdev,b_CounterNumber);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;

}

int do_CMD_xpci3xxx_StopAllCounters                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	int ret;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_StopAllCounters (pdev);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;

}


int do_CMD_xpci3xxx_ClearCounter                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	int ret;
	uint8_t   b_CounterNumber = 0;

	if ( copy_from_user( &b_CounterNumber, (uint8_t __user *) arg, sizeof(b_CounterNumber) ) )
		return -EFAULT;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_ClearCounter (pdev, b_CounterNumber);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;
}

int do_CMD_xpci3xxx_ReadCounterValue  (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{

	uint8_t   b_CounterNumber = 0;
	uint32_t dw_CounterValue;
	int ret;

	uint32_t dw_ArgTable[2];

	if ( copy_from_user( dw_ArgTable, (uint32_t __user *) arg, sizeof(dw_ArgTable) ) )
		return -EFAULT;

	b_CounterNumber = (uint8_t)dw_ArgTable[0];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret =  i_xpci3xxx_ReadCounterValue  (pdev,
					b_CounterNumber,
					&dw_CounterValue);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	if (ret)
		return ret;

	dw_ArgTable[1] = dw_CounterValue;

	/* Transfers the value to the user */
	if ( copy_to_user( (uint32_t __user *) arg , dw_ArgTable, sizeof(dw_ArgTable) ) )
		return -EFAULT;

	return 0;
}

int do_CMD_xpci3xxx_ReadCounterStatus  (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	/************************/
	/* Variable declaration */
	/************************/
	uint8_t   b_CounterNumber = 0;
	uint8_t b_CounterStatus = 0;
	uint8_t b_SoftwareTriggerStatus = 0;
	uint8_t b_HardwareTriggerStatus = 0;
	uint8_t b_SoftwareClearStatus = 0;

	uint8_t b_ArgTable[5];
	int ret;

	if ( copy_from_user( b_ArgTable, (uint8_t __user *) arg, sizeof(b_ArgTable) ) )
		return -EFAULT;

	b_CounterNumber = b_ArgTable[0];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_ReadCounterStatus  (pdev,
					b_CounterNumber,
					&b_CounterStatus,
					&b_SoftwareTriggerStatus,
					&b_HardwareTriggerStatus,
					&b_SoftwareClearStatus);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	if (ret)
		return ret;

	b_ArgTable[1] = b_CounterStatus;
	b_ArgTable[2] = b_SoftwareTriggerStatus;
	b_ArgTable[3] = b_HardwareTriggerStatus;
	b_ArgTable[4] = b_SoftwareClearStatus;

	/* Transfers the value to the user */
	if ( copy_to_user( (uint8_t __user *) arg , b_ArgTable, sizeof(b_ArgTable) ) )
		return -EFAULT;

	return 0;
}

int do_CMD_xpci3xxx_EnableDisableCounterInterrupt          (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t   b_CounterNumber = 0;
	uint8_t   b_InterruptFlag = 0;
	int ret;
	uint8_t   b_ArgTable[2];

	if ( copy_from_user( b_ArgTable, (uint8_t __user *) arg, sizeof(b_ArgTable) ) )
		return -EFAULT;

	b_CounterNumber = b_ArgTable[0];
	b_InterruptFlag = b_ArgTable[1];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_EnableDisableCounterInterrupt (pdev, b_CounterNumber, b_InterruptFlag);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}
	return ret;
}

int do_CMD_xpci3xxx_EnableDisableCounterHardwareOutput          (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	/************************/
	/* Variable declaration */
	/************************/
	uint8_t   b_CounterNumber = 0;
	uint8_t   b_OutputFlag = 0;
	uint8_t   b_OutputLevel = 0;
	int ret;
	uint8_t b_ArgTable[3];

	if ( copy_from_user( b_ArgTable, (uint8_t __user *) arg, sizeof(b_ArgTable) ) )
		return -EFAULT;

	b_CounterNumber = b_ArgTable[0];
	b_OutputFlag = b_ArgTable[1];
	b_OutputLevel = b_ArgTable[1];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_EnableDisableCounterHardwareOutput       (pdev,
					b_CounterNumber,
					b_OutputFlag,
					b_OutputLevel);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;

}

int do_CMD_xpci3xxx_GetCounterHardwareOutputStatus  (struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t   b_CounterNumber = 0;
	uint8_t b_HardwareOutputStatus;
	uint8_t b_ArgTable[2];
	int ret;

	if ( copy_from_user( b_ArgTable, (uint8_t __user *) arg, sizeof(b_ArgTable) ) )
		return -EFAULT;

	b_CounterNumber = b_ArgTable[0];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_GetCounterHardwareOutputStatus  (pdev,
					b_CounterNumber,
					&b_HardwareOutputStatus);
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
