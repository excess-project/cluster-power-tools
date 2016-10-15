/** @file analog_input.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
*
* @version  $LastChangedRevision$
* @version $LastChangedDate$
*
* Kernel functions for the analog input functionality of the board.
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


int do_CMD_xpci3xxx_InitAnalogInput (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t b_Channel;
	uint8_t b_SingleDiff;
	uint8_t b_Gain;
	uint8_t b_Polarity;
	uint8_t b_ArgTable[4];
	int ret;

	if ( copy_from_user( b_ArgTable, (uint8_t __user *) arg, sizeof(b_ArgTable) ) )
		return -EFAULT;

	b_Channel = b_ArgTable[0];
	b_SingleDiff = b_ArgTable[1];
	b_Gain = b_ArgTable[2];
	b_Polarity = b_ArgTable[3];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_InitAnalogInput(pdev,
									b_Channel,
									b_SingleDiff,
									b_Gain,
									b_Polarity);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}
	return ret;

}

int do_CMD_xpci3xxx_StartAnalogInput (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t   b_Channel;
	uint8_t   b_ConvertTimeUnit;
	uint16_t   w_ConvertTime;
	uint16_t   w_ArgTable[3];
	int ret;

	if ( copy_from_user( w_ArgTable, (uint16_t __user *) arg, sizeof(w_ArgTable) ) )
		return -EFAULT;

	b_Channel = (uint8_t)w_ArgTable[0];
	b_ConvertTimeUnit = (uint8_t)w_ArgTable[1];
	w_ConvertTime = w_ArgTable[2];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_StartAnalogInput(pdev,
									b_Channel,
									b_ConvertTimeUnit,
									w_ConvertTime);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}
	return ret;
}

int do_CMD_xpci3xxx_ReadAnalogInputBit (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t   b_AnalogInputConversionFinished;
	int ret = 0;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_ReadAnalogInputBit(pdev,&b_AnalogInputConversionFinished);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	if (ret)
		return ret;

	/* Transfers the value to the user */
	if ( copy_to_user( (uint8_t __user *) arg , &b_AnalogInputConversionFinished, sizeof(b_AnalogInputConversionFinished) ) )
		return -EFAULT;

	return 0;
}

int do_CMD_xpci3xxx_ReadAnalogInputValue (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t dw_ReadValue;
	int ret;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_ReadAnalogInputValue(pdev,&dw_ReadValue);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	if (ret)
		return ret;

	/* Transfers the value to the user */
	if ( copy_to_user( (uint32_t __user *) arg , &dw_ReadValue, sizeof(dw_ReadValue) ) )
		return -EFAULT;

	return 0;
}

int do_CMD_xpci3xxx_InitAnalogInputAutoRefresh(struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	//uint32_t* dw_ArgTable[(3 * ADDIDATA_MAX_AI) + 6];
	int ret = 0;

	uint32_t NumberOfChannels = 0;
	unsigned int size = (3 * ADDIDATA_MAX_AI) + 6; /* size of the array */
	uint32_t* tmpbuff = NULL;

    uint8_t   b_ChannelList[ADDIDATA_MAX_AI];
    uint8_t   b_Gain[ADDIDATA_MAX_AI];
	uint8_t   b_Polarity[ADDIDATA_MAX_AI];
	uint8_t   b_SingleDiff = 0;
	uint32_t  dw_NumberOfSequence = 0;
    uint8_t    b_DelayMode = 0;
    uint8_t    b_DelayTimeUnit = 0;
    uint16_t    w_DelayTime = 0;
	uint8_t   b_ChannelCounter = 0;

	/* first get NumberOfChannels */
	if ( copy_from_user( &NumberOfChannels, (uint32_t __user *) arg, sizeof(NumberOfChannels) ) )
		return -EFAULT;

	/* check number of channel */
	switch(NumberOfChannels)
	{
		case 1 ... ADDIDATA_MAX_AI:
			break;
		default:
			return 2;
	}
	size = ((3*NumberOfChannels) + 6) * sizeof(uint32_t);

	/* allocate parameters array */
	tmpbuff = kmalloc(size, GFP_KERNEL);
	if(!tmpbuff)
		return -ENOMEM;

	/* get parameter */
	if ( copy_from_user( tmpbuff, (uint32_t __user *) arg, size ) )
	{
		kfree(tmpbuff);
		return -EFAULT;
	}

	/* Save the channel list */
	for (b_ChannelCounter = 0 ; b_ChannelCounter < NumberOfChannels ; b_ChannelCounter++)
		b_ChannelList[b_ChannelCounter] = (uint8_t)tmpbuff[1 + b_ChannelCounter];

	/* Save the gain list */
	for (b_ChannelCounter = NumberOfChannels ; b_ChannelCounter < (2 * NumberOfChannels) ; b_ChannelCounter++)
		b_Gain[b_ChannelCounter - NumberOfChannels] = (uint8_t)tmpbuff[1 + b_ChannelCounter];

	/* Save the polarity list */
	for (b_ChannelCounter = (2 * NumberOfChannels) ; b_ChannelCounter < (3 * NumberOfChannels) ; b_ChannelCounter++)
		b_Polarity[b_ChannelCounter - (2 * NumberOfChannels)] = (uint8_t)tmpbuff[1 + b_ChannelCounter];

	/* Save the single diff parameter */
	b_SingleDiff = (uint8_t)tmpbuff[(3 * NumberOfChannels) + 1];

	/* Save the number of sequence */
	dw_NumberOfSequence = tmpbuff[(3 * NumberOfChannels) + 2];

	/* Save the delay mode parameter */
    b_DelayMode = (uint8_t)tmpbuff[(3 * NumberOfChannels) + 3];

	/* Save the delay time unit */
    b_DelayTimeUnit = (uint8_t)tmpbuff[(3 * NumberOfChannels) + 4];

	/* Save the delay time at offset  */
    w_DelayTime = (uint16_t)tmpbuff[(3 * NumberOfChannels) + 5];

    /* temporary buffer is no more needed */
    kfree(tmpbuff);

 	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_InitAnalogInputAutoRefresh (pdev,
		                                            NumberOfChannels,
		                                            b_ChannelList,
					                                b_Gain,
					                                b_Polarity,
					                                b_SingleDiff,
		                                           	dw_NumberOfSequence,
		                                            b_DelayMode,
		                                            b_DelayTimeUnit,
		                                            w_DelayTime);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;
}

int do_CMD_xpci3xxx_StartAnalogInputAutoRefresh (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint16_t   w_ArgTable[2];
	uint8_t   b_ConvertTimeUnit = 0;
	uint16_t   w_ConvertTime = 0;
	int ret;

	if ( copy_from_user( w_ArgTable, (uint16_t __user *) arg, sizeof(w_ArgTable) ) )
		return -EFAULT;

	b_ConvertTimeUnit = (uint8_t)w_ArgTable[0];
	w_ConvertTime = w_ArgTable[1];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_StartAnalogInputAutoRefresh (pdev,b_ConvertTimeUnit,w_ConvertTime);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;
}

int do_CMD_xpci3xxx_StopAnalogInputAutoRefresh (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int ret;
	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_StopAnalogInputAutoRefresh (pdev);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}
	return ret;
}

int do_CMD_xpci3xxx_ReadAnalogInputAutoRefreshValueAndCounter (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t tmpbuff[ADDIDATA_MAX_AI + 1];
	uint32_t   dw_CycleIndex = 0;
	int ret;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_ReadAnalogInputAutoRefreshValueAndCounter (pdev, tmpbuff, &dw_CycleIndex);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	if (ret)
		return ret;

	/* values are 16 bit */
	{
		int i;
		for (i = 0 ; i < ADDIDATA_MAX_AI; i++)
			tmpbuff[i] = tmpbuff[i] & 0xFFFF;
	}

	/* last field of array is counter */
	tmpbuff[ADDIDATA_MAX_AI]=dw_CycleIndex;

	/* Transfers the value to the user */
	if ( copy_to_user( (uint32_t __user *) arg , tmpbuff, sizeof(tmpbuff) ) )
		return -EFAULT;

	return 0;
}

int do_CMD_xpci3xxx_ReleaseAnalogInputAutoRefresh (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int ret;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_StopAnalogInputAutoRefresh (pdev);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	if (ret)
		return ret;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_ReleaseAnalogInputAutoRefresh (pdev);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}
	return ret;
}



int do_CMD_xpci3xxx_InitAnalogInputSequence (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint32_t NumberOfChannels = 0;
	unsigned int size = (3 * ADDIDATA_MAX_AI) + 8; /* max. size of the array */
	uint32_t* tmpbuff = NULL;

	uint8_t   b_ChannelList[ADDIDATA_MAX_AI];
	uint8_t   b_Gain[ADDIDATA_MAX_AI];
	uint8_t   b_Polarity[ADDIDATA_MAX_AI];
	uint8_t   b_SingleDiff = 0;
	uint32_t  dw_NumberOfSequence = 0;
	uint8_t    b_UseDMA = 0;
	uint32_t  dw_NumberOfSequenceForEachInterrupt = 0;
    uint8_t    b_DelayMode = 0;
    uint8_t    b_DelayTimeUnit = 0;
    uint16_t    w_DelayTime = 0;
	uint8_t   b_ChannelCounter = 0;
	int ret;


	/* first get NumberOfChannels */
	if ( copy_from_user( &NumberOfChannels, (uint32_t __user *) arg, sizeof(NumberOfChannels) ) )
		return -EFAULT;

	/* check number of channel */
	switch(NumberOfChannels)
	{
		case 1 ... ADDIDATA_MAX_AI:
			break;
		default:
			return 2;
	}
	size = ((3*NumberOfChannels) + 8) * sizeof(uint32_t);

	/* allocate parameters array */
	tmpbuff = kmalloc(size, GFP_KERNEL);
	if(!tmpbuff)
		return -ENOMEM;

	if ( copy_from_user( tmpbuff, (uint32_t __user *) arg, size ) )
	{
		kfree(tmpbuff);
		return -EFAULT;
	}

	/* Save the channel list */
	for (b_ChannelCounter = 0 ; b_ChannelCounter < NumberOfChannels ; b_ChannelCounter++)
		b_ChannelList[b_ChannelCounter] = (uint8_t)tmpbuff[1 + b_ChannelCounter];

	/* Save the gain list */
	for (b_ChannelCounter = NumberOfChannels ; b_ChannelCounter < (2 * NumberOfChannels) ; b_ChannelCounter++)
		b_Gain[b_ChannelCounter - NumberOfChannels] = (uint8_t)tmpbuff[1 + b_ChannelCounter];

	/* Save the polarity list */
	for (b_ChannelCounter = (2 * NumberOfChannels) ; b_ChannelCounter < (3 * NumberOfChannels) ; b_ChannelCounter++)
		b_Polarity[b_ChannelCounter - (2 * NumberOfChannels)] = (uint8_t)tmpbuff[1 + b_ChannelCounter];

	/* Save the single diff parameter */
	b_SingleDiff = (uint8_t)tmpbuff[(3 * NumberOfChannels) + 1];

	/* Save the number of sequence */
	dw_NumberOfSequence = tmpbuff[(3 * NumberOfChannels) + 2];

	/* Save the DMA use parameter */
	b_UseDMA = (uint8_t)tmpbuff[(3 * NumberOfChannels) + 3];

	/* Save the number of sequence for each interrupt */
	dw_NumberOfSequenceForEachInterrupt = tmpbuff[(3 * NumberOfChannels) + 4];

	/* Save the delay mode parameter */
    b_DelayMode = (uint8_t)tmpbuff[(3 * NumberOfChannels) + 5];

	/* Save the delay time unit */
    b_DelayTimeUnit = (uint8_t)tmpbuff[(3 * NumberOfChannels) + 6];

	/* Save the delay time */
    w_DelayTime = (uint16_t)tmpbuff[(3 * NumberOfChannels) + 7];

    /* temporary buffer no more needed */
    kfree(tmpbuff);

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_InitAnalogInputSequence (pdev,
                                            (uint8_t) NumberOfChannels,
                                            b_ChannelList,
			                                b_Gain,
			                                b_Polarity,
			                                b_SingleDiff,
                                            dw_NumberOfSequence,
											b_UseDMA,
											dw_NumberOfSequenceForEachInterrupt,
                                            b_DelayMode,
                                            b_DelayTimeUnit,
                                            w_DelayTime);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}
	return ret;
}




int do_CMD_xpci3xxx_StartAnalogInputSequence (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint16_t   w_ArgTable[2];
	uint8_t   b_ConvertTimeUnit = 0;
	uint16_t   w_ConvertTime = 0;
	int ret;

	if ( copy_from_user( w_ArgTable, (uint16_t __user *) arg, sizeof(w_ArgTable) ) )
		return -EFAULT;

	b_ConvertTimeUnit = (uint8_t)w_ArgTable[0];
	w_ConvertTime = w_ArgTable[1];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_StartAnalogInputSequence (pdev, b_ConvertTimeUnit, w_ConvertTime);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;
}




int do_CMD_xpci3xxx_StopAnalogInputSequence (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int ret;
	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_StopAnalogInputSequence (pdev);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;
}


int do_CMD_xpci3xxx_ReleaseAnalogInputSequence (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int ret;
	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_ReleaseAnalogInputSequence (pdev);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}
	return ret;
}

int do_CMD_xpci3xxx_EnableDisableAnalogInputHardwareTrigger (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{

	uint32_t  dw_ArgTable[4];
	uint8_t   b_HardwareTrigger = 0;
	uint8_t   b_HardwareTriggerLevel = 0;
	uint8_t   b_HardwareTriggerAction = 0;
	uint32_t  dw_HardwareTriggerCount = 0;
	int ret;

	if ( copy_from_user( dw_ArgTable, (uint32_t __user *) arg, sizeof(dw_ArgTable) ) )
		return -EFAULT;

	b_HardwareTrigger = (uint8_t)dw_ArgTable[0];
	b_HardwareTriggerLevel = (uint8_t)dw_ArgTable[1];
	b_HardwareTriggerAction = (uint8_t)dw_ArgTable[2];
	dw_HardwareTriggerCount = dw_ArgTable[3];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_EnableDisableAnalogInputHardwareTrigger (pdev,
								                                b_HardwareTrigger,
																b_HardwareTriggerLevel,
																b_HardwareTriggerAction,
																dw_HardwareTriggerCount);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}
	return ret;

}




int do_CMD_xpci3xxx_GetAnalogInputHardwareTriggerStatus (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{

	uint32_t dw_ArgTable[3];
	uint8_t   b_HardwareTriggerStatus = 0;
	uint32_t  dw_HardwareTriggerCount = 0;
	uint8_t   b_HardwareTriggerState = 0;
	int ret;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_GetAnalogInputHardwareTriggerStatus (pdev,
										              &b_HardwareTriggerStatus,
										              &dw_HardwareTriggerCount,
										              &b_HardwareTriggerState);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	if (ret)
		return ret;

	dw_ArgTable[0] = (uint32_t)b_HardwareTriggerStatus;
	dw_ArgTable[1] = dw_HardwareTriggerCount;
	dw_ArgTable[2] = (uint32_t)b_HardwareTriggerState;

	/* Transfers the value to the user */
	if ( copy_to_user( (uint32_t __user *) arg , dw_ArgTable, sizeof(dw_ArgTable) ) )
		return -EFAULT;

	return 0;
}




int do_CMD_xpci3xxx_EnableDisableAnalogInputSoftwareTrigger (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	uint8_t   b_ArgTable[2];
	uint8_t   b_HardwareTrigger = 0;
	uint8_t   b_HardwareTriggerAction = 0;
	int ret;

	if ( copy_from_user( b_ArgTable, (uint8_t __user *) arg, sizeof(b_ArgTable) ) )
		return -EFAULT;

	b_HardwareTrigger = b_ArgTable[0];
	b_HardwareTriggerAction = b_ArgTable[1];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_EnableDisableAnalogInputSoftwareTrigger (pdev, b_HardwareTrigger, b_HardwareTriggerAction);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}
	return ret;

}

int do_CMD_xpci3xxx_AnalogInputSoftwareTrigger (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{
	int ret;
	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_AnalogInputSoftwareTrigger(pdev);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	return ret;

}

int do_CMD_xpci3xxx_GetAnalogInputSoftwareTriggerStatus (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{

	uint8_t   b_SoftwareTriggerStatus = 0;
	int ret;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_GetAnalogInputSoftwareTriggerStatus (pdev, &b_SoftwareTriggerStatus);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	if (ret)
		return ret;

	/* Transfers the value to the user */
	if ( copy_to_user( (uint8_t __user *) arg , &b_SoftwareTriggerStatus, sizeof(b_SoftwareTriggerStatus) ) )
		return -EFAULT;

	return 0;
}



int do_CMD_xpci3xxx_EnableDisableAnalogInputHardwareGate (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{

	uint8_t   b_ArgTable[2];
	uint8_t   b_HardwareGate = 0;
	uint8_t   b_HardwareGateLevel = 0;
	int ret;

	if ( copy_from_user( b_ArgTable, (uint8_t __user *) arg, sizeof(b_ArgTable) ) )
		return -EFAULT;

	b_HardwareGate = b_ArgTable[0];
	b_HardwareGateLevel = b_ArgTable[1];

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_EnableDisableAnalogInputHardwareGate (pdev, b_HardwareGate, b_HardwareGateLevel);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}
	return ret;

}

int do_CMD_xpci3xxx_GetAnalogInputHardwareGateStatus (struct pci_dev *pdev, unsigned int cmd, unsigned long arg)
{

	uint8_t  b_HardwareGateStatus = 0;
	int ret;

	{
		unsigned long irqstate;
		XPCI3XXX_LOCK(pdev,&irqstate);
		{
			ret = i_xpci3xxx_GetAnalogInputHardwareGateStatus (pdev, &b_HardwareGateStatus);
		}
		XPCI3XXX_UNLOCK(pdev,irqstate);
	}

	if (ret)
		return ret;

	/* Transfers the value to the user */
	if ( copy_to_user( (uint8_t __user *) arg , &b_HardwareGateStatus, sizeof(b_HardwareGateStatus) ) )
		return -EFAULT;

	return 0;
}

