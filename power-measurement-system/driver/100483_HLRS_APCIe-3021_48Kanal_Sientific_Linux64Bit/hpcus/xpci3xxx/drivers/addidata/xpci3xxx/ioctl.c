/** @file ioctl.c
*
* @author Sylvain Nahas
*
* @date 05.01.06
*
* @version  $LastChangedRevision$
* @version $LastChangedDate$
*
* This module implements the xpci3xxx_do_ioctl and the GetHardwareInformation functions
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

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/pci.h>
#include <asm/io.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0)
	#include <asm/system.h>
#endif
#include <asm/uaccess.h>
#include <asm/bitops.h>
#include <linux/sched.h>

#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/delay.h>

#include "xpci3xxx-private.h"
#include "vtable.h"
//#include "privdata.h"

/**@def EXPORT_NO_SYMBOLS
 * Function in this file are not exported.
 */
EXPORT_NO_SYMBOLS;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,27)
#define __user
#endif

//------------------------------------------------------------------------------
/** dummy command to be called when a ioctl command is incorrect */
static int xpci3xxx_do_dummy(struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	printk (KERN_WARNING "%s: %d: invalid ioctl\n",__DRIVER_NAME,_IOC_NR(cmd));
	return -EINVAL;
}
//------------------------------------------------------------------------------
/** add new ioctl handlers here */
void xpci3xxx_init_vtable(vtable_t vtable)
{

	xpci3xxx_dummyze_vtable(vtable, xpci3xxx_do_dummy);


	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_GetHardwareInformation, do_CMD_xpci3xxx_GetHardwareInformation);

	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_TestInterrupt, do_CMD_xpci3xxx_TestInterrupt);

	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_SetTTLPortConfiguration, do_CMD_xpci3xxx_SetTTLPortConfiguration);


	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_InitAnalogInput , do_CMD_xpci3xxx_InitAnalogInput);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_StartAnalogInput , do_CMD_xpci3xxx_StartAnalogInput);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_ReadAnalogInputBit , do_CMD_xpci3xxx_ReadAnalogInputBit);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_ReadAnalogInputValue , do_CMD_xpci3xxx_ReadAnalogInputValue);

	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_InitAnalogInputAutoRefresh , do_CMD_xpci3xxx_InitAnalogInputAutoRefresh);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_StartAnalogInputAutoRefresh , do_CMD_xpci3xxx_StartAnalogInputAutoRefresh);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_StopAnalogInputAutoRefresh , do_CMD_xpci3xxx_StopAnalogInputAutoRefresh);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_ReadAnalogInputAutoRefreshValueAndCounter , do_CMD_xpci3xxx_ReadAnalogInputAutoRefreshValueAndCounter);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_ReleaseAnalogInputAutoRefresh , do_CMD_xpci3xxx_ReleaseAnalogInputAutoRefresh);

	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_InitAnalogInputSequence , do_CMD_xpci3xxx_InitAnalogInputSequence);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_StartAnalogInputSequence , do_CMD_xpci3xxx_StartAnalogInputSequence);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_StopAnalogInputSequence , do_CMD_xpci3xxx_StopAnalogInputSequence);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_ReleaseAnalogInputSequence , do_CMD_xpci3xxx_ReleaseAnalogInputSequence);

	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_EnableDisableAnalogInputHardwareTrigger , do_CMD_xpci3xxx_EnableDisableAnalogInputHardwareTrigger);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_GetAnalogInputHardwareTriggerStatus , do_CMD_xpci3xxx_GetAnalogInputHardwareTriggerStatus);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_EnableDisableAnalogInputSoftwareTrigger , do_CMD_xpci3xxx_EnableDisableAnalogInputSoftwareTrigger);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_AnalogInputSoftwareTrigger , do_CMD_xpci3xxx_AnalogInputSoftwareTrigger);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_GetAnalogInputSoftwareTriggerStatus , do_CMD_xpci3xxx_GetAnalogInputSoftwareTriggerStatus);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_EnableDisableAnalogInputHardwareGate , do_CMD_xpci3xxx_EnableDisableAnalogInputHardwareGate);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_GetAnalogInputHardwareGateStatus , do_CMD_xpci3xxx_GetAnalogInputHardwareGateStatus);

	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_InitAnalogOutput , do_CMD_xpci3xxx_InitAnalogOutput);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_ReadAnalogOutputBit , do_CMD_xpci3xxx_ReadAnalogOutputBit);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_WriteAnalogOutputValue , do_CMD_xpci3xxx_WriteAnalogOutputValue);

	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_Read32DigitalInputs, do_CMD_xpci3xxx_Read32DigitalInputs);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_InitDigitalInputModuleFilter, do_CMD_xpci3xxx_InitDigitalInputModuleFilter);

	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_Set32DigitalOutputsOn , do_CMD_xpci3xxx_Set32DigitalOutputsOn);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_Set32DigitalOutputsOff, do_CMD_xpci3xxx_Set32DigitalOutputsOff);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_SetDigitalOutputMemoryOn, do_CMD_xpci3xxx_SetDigitalOutputMemoryOn);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_SetDigitalOutputMemoryOff, do_CMD_xpci3xxx_SetDigitalOutputMemoryOff);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_Get32DigitalOutputStatus, do_CMD_xpci3xxx_Get32DigitalOutputStatus);

/*	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_SetIntCallback, do_CMD_xpci3xxx_SetIntCallback);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_ResetIntCallback, do_CMD_xpci3xxx_ResetIntCallback);	*/

	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_EnableDisableTimerHardwareTrigger, do_CMD_xpci3xxx_EnableDisableTimerHardwareTrigger);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_InitTimer, do_CMD_xpci3xxx_InitTimer);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_ReleaseTimer, do_CMD_xpci3xxx_ReleaseTimer);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_StartTimer, do_CMD_xpci3xxx_StartTimer);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_StartAllTimers, do_CMD_xpci3xxx_StartAllTimers);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_TriggerTimer, do_CMD_xpci3xxx_TriggerTimer);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_TriggerAllTimers, do_CMD_xpci3xxx_TriggerAllTimers);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_StopTimer, do_CMD_xpci3xxx_StopTimer);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_StopAllTimers, do_CMD_xpci3xxx_StopAllTimers);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_ReadTimerValue, do_CMD_xpci3xxx_ReadTimerValue);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_ReadTimerStatus, do_CMD_xpci3xxx_ReadTimerStatus);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_EnableDisableTimerInterrupt, do_CMD_xpci3xxx_EnableDisableTimerInterrupt);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_EnableDisableTimerHardwareOutput, do_CMD_xpci3xxx_EnableDisableTimerHardwareOutput);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_GetTimerHardwareOutputStatus, do_CMD_xpci3xxx_GetTimerHardwareOutputStatus);

	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_InitCounter, do_CMD_xpci3xxx_InitCounter);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_ReleaseCounter, do_CMD_xpci3xxx_ReleaseCounter);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_StartCounter, do_CMD_xpci3xxx_StartCounter);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_StartAllCounters, do_CMD_xpci3xxx_StartAllCounters);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_TriggerCounter, do_CMD_xpci3xxx_TriggerCounter);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_TriggerAllCounters, do_CMD_xpci3xxx_TriggerAllCounters);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_StopCounter, do_CMD_xpci3xxx_StopCounter);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_StopAllCounters, do_CMD_xpci3xxx_StopAllCounters);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_ClearCounter, do_CMD_xpci3xxx_ClearCounter);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_ReadCounterValue, do_CMD_xpci3xxx_ReadCounterValue);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_ReadCounterStatus, do_CMD_xpci3xxx_ReadCounterStatus);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_EnableDisableCounterInterrupt, do_CMD_xpci3xxx_EnableDisableCounterInterrupt);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_EnableDisableCounterHardwareOutput, do_CMD_xpci3xxx_EnableDisableCounterHardwareOutput);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_GetCounterHardwareOutputStatus, do_CMD_xpci3xxx_GetCounterHardwareOutputStatus);

	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_InitWatchdog, do_CMD_xpci3xxx_InitWatchdog);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_ReleaseWatchdog, do_CMD_xpci3xxx_ReleaseWatchdog);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_StartWatchdog, do_CMD_xpci3xxx_StartWatchdog);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_StartAllWatchdogs, do_CMD_xpci3xxx_StartAllWatchdogs);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_TriggerWatchdog, do_CMD_xpci3xxx_TriggerWatchdog);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_TriggerAllWatchdogs, do_CMD_xpci3xxx_TriggerAllWatchdogs);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_StopWatchdog, do_CMD_xpci3xxx_StopWatchdog);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_StopAllWatchdogs, do_CMD_xpci3xxx_StopAllWatchdogs);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_ReadWatchdogStatus, do_CMD_xpci3xxx_ReadWatchdogStatus);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_ReadWatchdogValue, do_CMD_xpci3xxx_ReadWatchdogValue);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_EnableDisableWatchdogInterrupt, do_CMD_xpci3xxx_EnableDisableWatchdogInterrupt);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_EnableDisableWatchdogHardwareOutput, do_CMD_xpci3xxx_EnableDisableWatchdogHardwareOutput);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_GetWatchdogHardwareOutputStatus, do_CMD_xpci3xxx_GetWatchdogHardwareOutputStatus);

	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_OUTPORTDW, do_CMD_xpci3xxx_OUTPORTDW);
	__xpci3xxx_DECLARE_IOCTL_HANDLER(vtable, CMD_xpci3xxx_INPORTDW, do_CMD_xpci3xxx_INPORTDW);
}

//------------------------------------------------------------------------------
int xpci3xxx_do_ioctl(struct pci_dev * pdev,unsigned int cmd,unsigned long arg)
{

//	printk("%s\n",__FUNCTION__); // __FILE__,__FUNCTION__,__LINE__

	/* boundaries check
	 *
	 * VTABLE_ELEMENT_NB(vtable_t) = __xpci3xxx_UPPER_IOCTL_CMD +1
	 * maximum index value = __xpci3xxx_UPPER_IOCTL_CMD = VTABLE_ELEMENT_NB(vtable_t) -1
	 *
	 * the idea here is to favorize compilation-time
	 *
	 * */

	if (_IOC_NR(cmd) > (VTABLE_ELEMENT_NB(vtable_t)-1) )
		return xpci3xxx_do_dummy(pdev,cmd,arg);

	/* call actual ioctl handler - should be safe now */
	return (xpci3xxx_vtable[_IOC_NR(cmd)]) (pdev, cmd, arg);
}
//------------------------------------------------------------------------------
int do_CMD_xpci3xxx_GetHardwareInformation(struct pci_dev * pdev, unsigned int cmd, unsigned long arg)
{
	struct xpci3xxx_USER_str_BoardInformations xpci3xxx_USER_s_BoardInformations;
	xpci3xxx_USER_s_BoardInformations.dw_BoardBaseAddress[0] = (unsigned long) GET_MEM_BAR0(pdev);
	xpci3xxx_USER_s_BoardInformations.dw_BoardBaseAddress[1] = GET_BAR1(pdev);
	xpci3xxx_USER_s_BoardInformations.dw_BoardBaseAddress[2] = GET_BAR2(pdev);
	xpci3xxx_USER_s_BoardInformations.dw_BoardBaseAddress[3] = (unsigned long) GET_MEM_BAR3(pdev);
	xpci3xxx_USER_s_BoardInformations.b_InterruptNbr = pdev->irq;
	xpci3xxx_USER_s_BoardInformations.b_SlotNumber = PCI_SLOT(pdev->devfn);

	if ( copy_to_user ((struct xpci3xxx_USER_str_BoardInformations __user *)arg,&xpci3xxx_USER_s_BoardInformations, sizeof(struct xpci3xxx_USER_str_BoardInformations)) )
		return -EFAULT;

	return 0;
}

