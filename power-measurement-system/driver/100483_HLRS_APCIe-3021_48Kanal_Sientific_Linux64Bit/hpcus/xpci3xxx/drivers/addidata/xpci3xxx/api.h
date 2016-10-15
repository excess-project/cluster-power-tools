/** @file api.h
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
* 
* @version  $LastChangedRevision$
* @version $LastChangedDate$
* 
* Kernel functions prototype for all functionality of the board.
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

#ifndef __xpci3xxx_API_H_
#define __xpci3xxx_API_H_

int do_CMD_xpci3xxx_GetHardwareInformation(struct pci_dev * pdev, unsigned int cmd, unsigned long arg);

int do_CMD_xpci3xxx_TestInterrupt(struct pci_dev * pdev, unsigned int cmd, unsigned long arg);

int do_CMD_xpci3xxx_SetTTLPortConfiguration(struct pci_dev * pdev, unsigned int cmd, unsigned long arg);

int do_CMD_xpci3xxx_InitAnalogInput (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_StartAnalogInput (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_ReadAnalogInputBit (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_ReadAnalogInputValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

int do_CMD_xpci3xxx_InitAnalogInputAutoRefresh (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_StartAnalogInputAutoRefresh (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_StopAnalogInputAutoRefresh (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_ReadAnalogInputAutoRefreshValueAndCounter (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_ReleaseAnalogInputAutoRefresh (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

int do_CMD_xpci3xxx_InitAnalogInputSequence (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_StartAnalogInputSequence (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_StopAnalogInputSequence (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_ReleaseAnalogInputSequence (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

int do_CMD_xpci3xxx_EnableDisableAnalogInputHardwareTrigger (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_GetAnalogInputHardwareTriggerStatus (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_EnableDisableAnalogInputSoftwareTrigger (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_AnalogInputSoftwareTrigger (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_GetAnalogInputSoftwareTriggerStatus (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_EnableDisableAnalogInputHardwareGate (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_GetAnalogInputHardwareGateStatus (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

int do_CMD_xpci3xxx_InitAnalogOutput (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_ReadAnalogOutputBit (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_WriteAnalogOutputValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

int do_CMD_xpci3xxx_Read32DigitalInputs(struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_InitDigitalInputModuleFilter(struct pci_dev * pdev, unsigned int cmd, unsigned long arg);

int do_CMD_xpci3xxx_Set32DigitalOutputsOn(struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_Set32DigitalOutputsOff(struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_SetDigitalOutputMemoryOn(struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_SetDigitalOutputMemoryOff(struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_Get32DigitalOutputStatus(struct pci_dev * pdev, unsigned int cmd, unsigned long arg);

int do_CMD_xpci3xxx_EnableDisableTimerHardwareTrigger (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_InitTimer                      (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_ReleaseTimer                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_StartTimer                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_StartAllTimers                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_TriggerTimer                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_TriggerAllTimers                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_StopTimer                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_StopAllTimers                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_ReadTimerValue  (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_ReadTimerStatus  (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_EnableDisableTimerInterrupt          (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_EnableDisableTimerHardwareOutput (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_GetTimerHardwareOutputStatus (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);

int do_CMD_xpci3xxx_InitCounter                      (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_ReleaseCounter                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_StartCounter                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_StartAllCounters                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_TriggerCounter                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_TriggerAllCounters                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_StopCounter                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_StopAllCounters                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_ClearCounter                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_ReadCounterValue  (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_ReadCounterStatus  (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_EnableDisableCounterInterrupt          (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_EnableDisableCounterHardwareOutput (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_GetCounterHardwareOutputStatus (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);

int do_CMD_xpci3xxx_InitWatchdog                      (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_ReleaseWatchdog                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_StartWatchdog                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_StartAllWatchdogs                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_TriggerWatchdog                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_TriggerAllWatchdogs                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_StopWatchdog                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_StopAllWatchdogs                    (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_ReadWatchdogValue  (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_ReadWatchdogStatus  (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_EnableDisableWatchdogInterrupt          (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_EnableDisableWatchdogHardwareOutput (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_GetWatchdogHardwareOutputStatus (struct pci_dev * pdev, unsigned int cmd, unsigned long arg);

int do_CMD_xpci3xxx_OUTPORTDW (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);
int do_CMD_xpci3xxx_INPORTDW (struct pci_dev *pdev, unsigned int cmd, unsigned long arg);

/** Execute a command.
*
* @param pdev Pointer to a device.
* @param cmd Command to execute
* @param arg Argument of the command.
*/
int xpci3xxx_do_ioctl(struct pci_dev * pdev, unsigned int cmd, unsigned long arg);
#endif
