/** @file primary_tcw.h
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
*
* @version  $LastChangedRevision$
* @version $LastChangedDate$
*
* Holds definition for driver's private data and its manipulation functions.
*
*/

/** @par LICENCE
*  @verbatim
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

#ifndef __XPCI3XXX_PRIVDATA_H_
#define __XPCI3XXX_PRIVDATA_H_

//#define DMA_MAX_SCATTER_GATHER_BUFFER          10000
//#define ADDIDATA_MAX_SCATTER_GATHER_SUB_BUFFER 10000
//#define ADDIDATA_MAX_DESCRIPTORS_LIST          10000
#define DMA_MAX_SCATTER_GATHER_BUFFER          100
#define ADDIDATA_MAX_SCATTER_GATHER_SUB_BUFFER 100
#define ADDIDATA_MAX_DESCRIPTORS_LIST          100

//#define ADDIDATA_RING_3 				0
//#define ADDIDATA_RING_0 				1
//
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,27)
#define __user
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#define __iomem
#endif

typedef struct
{
	struct
	{
		struct
		{
			caddr_t BigphysareaHandle;
			void * pv_DMABuffer;
			uint32_t  ul_PhysicalAddress;
			uint32_t  ul_BufferSize;
		} s_ScatterGather [DMA_MAX_SCATTER_GATHER_BUFFER];
		uint32_t  ul_ScatterGatherNbr;
	} s_BufferInformations [2];
	uint32_t  ul_BufferSize;
	uint32_t  ul_BufferNbr;
	uint32_t  ul_Status;
}
str_ScatterGatherPCIDMAInformation;

/* struct definition */
typedef struct
{
	uint8_t   b_Interrupt;                   // 0 : Buffer interrupt not generated
	// 1 : Buffer interrupt generated
	uint32_t dw_Size;                        // Single scatter gather buffer size
	uint32_t dw_PhysicalAddress;             // Single scatter gather buffer address
	caddr_t BigphysareaHandle;
	union
	{
		uint8_t  *  pb_DMABuffer;             // buffer address (byte)
		uint16_t  *  pw_DMABuffer;             // buffer address (word)
		uint32_t * pdw_DMABuffer;             // buffer address (dword)
	}s_Buffer;
}str_ScatterGatherSingleBuffer,*pstr_ScatterGatherSingleBuffer;

typedef struct
{
	uint32_t dw_AquisitionTotalSize;		// Acquisition total buffer size
	uint8_t   b_AcquisitionMode;			// Acquisition mode
	//   0 : Single acquisition
	//   1 : Continuous acquisition
	uint8_t   b_LocalBusWidth;				// Local bus width
	// 8  :  8-bit access
	// 16 : 16-bit access
	// 32 : 32-bit access
	uint8_t   b_DMAChannel;				// Selected DMA channel
	uint8_t   b_TransferStatus;			// 0 : Not selected
	// 1 : Disabled
	// 2 : Enabled
	// 3 : Started
	// 4 : Pause
	uint32_t dw_FisrtDescriptorAddress;									// First descriptor physical address
	void* pv_FreeDescriptorArrayAddress[ADDIDATA_MAX_DESCRIPTORS_LIST];	// Free descriptor address
	uint32_t dw_FreeDescriptorArraySize[ADDIDATA_MAX_DESCRIPTORS_LIST];	// Free descriptor size
	uint8_t   b_NumberOfDescriptorsList;									// Number of Descriptors list
	uint32_t dw_NumberOfScatterGatherBuffer;								// Number of scatter gather buffers
	str_ScatterGatherSingleBuffer s_ScatterGatherSingleBuffer [1];
}
str_ScatterGatherInterruptDescriptor,*pstr_ScatterGatherInterruptDescriptor;

/* internal driver data */
struct xpci3xxx_str_BoardInformations
{
	spinlock_t lock; /**< protect the board data */

	/* field used to implement linked list */
	struct pci_dev * previous; /**< previous in known-devices linked list */
	struct pci_dev * next; /**< next in known-devices linked list */

	int	    i_MinorNumber;
	uint8_t    b_InterruptInstalled;

	uint32_t	dw_InterruptSource[ADDIDATA_MAX_EVENT_COUNTER]; /* store the source of interrupt */
	uint32_t	dw_ChannelValue[ADDIDATA_MAX_EVENT_COUNTER][ADDIDATA_MAX_AI + 1];
	uint32_t	dw_SequenceArray[ADDIDATA_MAX_EVENT_COUNTER][ADDIDATA_MAX_INTERRUPT_ANALOG_INPUT_VALUE];
	uint32_t	dw_Timestamp[ADDIDATA_MAX_EVENT_COUNTER][ADDIDATA_MAX_INTERRUPT_ANALOG_INPUT_VALUE];
	uint32_t	dw_EventCounterWrite;
	uint32_t	dw_EventCounterRead;

	uint8_t	b_DigitalOutputMemoryState;
	uint32_t	dw_DigitalOutputRegister;

	void __iomem * dw_MemBaseAddress0;
	void __iomem * dw_MemBaseAddress3;

	uint8_t	b_TimerCounterWatchdogConfigurableAsTimer[ADDIDATA_MAX_TCW];
	uint8_t	b_TimerCounterWatchdogConfigurableAsCounter[ADDIDATA_MAX_TCW];
	uint8_t	b_TimerCounterWatchdogConfigurableAsWatchdog[ADDIDATA_MAX_TCW];

	/* Save the reload value in us for the jitter timer */
	uint32_t dw_ReloadValue;

	uint8_t	b_TimerCounterWatchdogConfiguration[ADDIDATA_MAX_TCW];
	uint32_t	dw_TimerCounterWatchdogOffsetAdress[ADDIDATA_MAX_TCW];

	uint8_t	b_DigitalInOutputSupported; /* 24V dig I/O  0 : boolean */
	uint32_t	dw_DigitalInputOffsetAdress;
	uint32_t	dw_DigitalOutputOffsetAdress;

	uint8_t	b_TTLSupported; /* TTL I/O  0 : boolean */
	uint32_t	dw_TTLOffsetAdress;
	uint8_t	b_TTLConfiguration;

	uint8_t   	b_NumberOfAnalogInput; /* analog input number : (0,ADDIDATA_MAX_AI) */
	uint8_t 	b_AnalogInputChannelInitialised[ADDIDATA_MAX_AI];

	uint8_t   	b_AnalogInputSeDiffConfigurationSupported; /* is configuration of single/differential is supported by the board ? */
	uint8_t   	b_AnalogInputSeDiffConfiguration; /* current configuration (constant if b_AnalogInputSeDiffConfigurationSupported is false) */

	uint32_t 	dw_AnalogInputOffsetAdress;
	uint32_t 	dw_MaxConvertTimeForNanoSecond;
	uint32_t 	dw_MinConvertTimeForNanoSecond;
	uint32_t 	dw_MaxConvertTimeForMicroSecond;
	uint32_t 	dw_MinConvertTimeForMicroSecond;
	uint32_t 	dw_MaxConvertTimeForMilliSecond;
	uint32_t 	dw_MinConvertTimeForMilliSecond;

	uint32_t 	dw_MaxDelayTimeForMicroSecond;
	uint32_t 	dw_MinDelayTimeForMicroSecond;
	uint32_t 	dw_MaxDelayTimeForMilliSecond;
	uint32_t 	dw_MinDelayTimeForMilliSecond;
	uint32_t 	dw_MaxDelayTimeForSecond;
	uint32_t 	dw_MinDelayTimeForSecond;

	uint8_t	b_AnalogInputAutoRefreshInitialised;
	uint8_t	b_NumberOfAutoRefreshChannels;

	uint8_t	b_AnalogInputSequenceInitialised;
	uint8_t	b_AnalogInputSequenceStarted;
	uint8_t	b_NumberOfSequenceChannels;

	uint8_t	b_AnalogInputDMASupported; /* analog input support DMA */
	uint8_t	b_AnalogInputDMA;
	str_ScatterGatherPCIDMAInformation     s_ScatterGatherPCIDMAInformation;
	str_ScatterGatherInterruptDescriptor  *ps_ScatterGatherDescriptor;
	uint32_t	dw_LastScatterGatherBuffer;
	uint32_t	dw_NumberOfSequenceForEachInterrupt;

	/* Variable for the shared memory */
	//struct fasync_struct * fa;
	char* kmalloc_area;
	char* kmalloc_ptr;

	uint8_t 	b_NumberOfAnalogOutput;	/* number of analog outputs */
	uint32_t	dw_AnalogOutputOffsetAdress;
	uint8_t 	b_NumberOfAnalogOutputVoltageMode;
	uint8_t 	b_AnalogOutputInitialisedVoltageMode;

	struct fasync_struct * async_queue; /* asynchronous readers */
	wait_queue_head_t wq;
};

/** initialise board's private data - fill it when adding new members and ioctl handlers */
static __inline__ void print_capability(struct pci_dev *pdev, char* str)
{
	printk(KERN_INFO "%s: board %s: capability %s\n", __DRIVER_NAME, pci_name(pdev), str );
}

static __inline__ void xpci3xxx_init_priv_data(struct pci_dev * pdev, struct xpci3xxx_str_BoardInformations * data)
{
	memset(data,0,sizeof(data));

	spin_lock_init(& (data->lock) );

	// FILLME
	data->async_queue = NULL;

	data->previous = NULL;
	data->next = NULL;

	data->i_MinorNumber = 0;
	data->b_InterruptInstalled = 0;

	memset (&data->dw_InterruptSource, 0, sizeof (data->dw_InterruptSource));
	memset (&data->dw_ChannelValue, 0, sizeof (data->dw_ChannelValue));
	memset (&data->dw_SequenceArray, 0, sizeof (data->dw_SequenceArray));
	data->dw_EventCounterWrite = 0;
	data->dw_EventCounterRead = 0;

	data->b_DigitalOutputMemoryState = 0;
	data->dw_DigitalOutputRegister = 0;

	data->dw_MemBaseAddress0 = NULL;
	data->dw_MemBaseAddress3 = NULL;

	memset (&data->b_AnalogInputChannelInitialised, 0, sizeof (data->b_AnalogInputChannelInitialised));
	memset (&data->b_TimerCounterWatchdogConfiguration, 0, sizeof (data->b_TimerCounterWatchdogConfiguration));

	data->b_AnalogInputAutoRefreshInitialised = 0;
	data->b_NumberOfAutoRefreshChannels = 0;

	data->b_AnalogInputSequenceInitialised = 0;
	data->b_AnalogInputSequenceStarted = 0;
	data->b_NumberOfSequenceChannels = 0;

	data->b_AnalogInputDMASupported = ADDIDATA_DISABLE;
	data->dw_LastScatterGatherBuffer = 0;
	data->dw_NumberOfSequenceForEachInterrupt = 0;

	//data->fa = NULL;
	data->kmalloc_area = NULL;
	data->kmalloc_ptr = NULL;
	data->ps_ScatterGatherDescriptor = NULL;
	memset (&data->s_ScatterGatherPCIDMAInformation, 0, sizeof (data->s_ScatterGatherPCIDMAInformation));

	//board capability

	// CASE OF THE xPCIe-3021
	if ((pdev->device == xpcie3021_16_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3021_8_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3021_4_BOARD_DEVICE_ID))
	{
		uint8_t b_TCWCounter = 0;
		print_capability(pdev, "xPCIe-3021");

		for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter++)
			data->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter] = 0;

		data->dw_TimerCounterWatchdogOffsetAdress[0] = 128;
		data->b_TimerCounterWatchdogConfigurableAsTimer[0] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[0] = ADDIDATA_DISABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[0] = ADDIDATA_DISABLE;

		data->b_DigitalInOutputSupported = ADDIDATA_ENABLE;
		data->dw_DigitalInputOffsetAdress = 32;
		data->dw_DigitalOutputOffsetAdress = 48;

		data->b_TTLSupported = ADDIDATA_DISABLE;
		data->dw_TTLOffsetAdress = 0;
		data->b_TTLConfiguration = 0;

		switch (pdev->device)
		{
			case xpcie3021_16_BOARD_DEVICE_ID:
				data->b_NumberOfAnalogInput = 16;
				data->b_NumberOfAnalogOutput = 0;
				break;
			case xpcie3021_8_BOARD_DEVICE_ID:
				data->b_NumberOfAnalogInput = 8;
				data->b_NumberOfAnalogOutput = 0;
				break;
			case xpcie3021_4_BOARD_DEVICE_ID:
				data->b_NumberOfAnalogInput = 4;
				data->b_NumberOfAnalogOutput = 0;
				break;
		}

		data->dw_AnalogOutputOffsetAdress = 96;
		data->b_NumberOfAnalogOutputVoltageMode = 2;
		data->b_AnalogOutputInitialisedVoltageMode = 0;

		data->dw_AnalogInputOffsetAdress = 0;
		data->b_AnalogInputSeDiffConfigurationSupported = ADDIDATA_ENABLE;
		data->b_AnalogInputSeDiffConfiguration = ADDIDATA_SINGLE;

		data->dw_MaxConvertTimeForNanoSecond  = 0;
		data->dw_MinConvertTimeForNanoSecond  = 0;
		data->dw_MaxConvertTimeForMicroSecond = 65535;
		data->dw_MinConvertTimeForMicroSecond = 10;
		data->dw_MaxConvertTimeForMilliSecond = 65535;
		data->dw_MinConvertTimeForMilliSecond = 1;

		data->dw_MaxDelayTimeForMicroSecond = 65535;
		data->dw_MinDelayTimeForMicroSecond = 10;
		data->dw_MaxDelayTimeForMilliSecond = 65535;
		data->dw_MinDelayTimeForMilliSecond = 1;
		data->dw_MaxDelayTimeForSecond  = 0;
		data->dw_MinDelayTimeForSecond  = 0;

		data->b_AnalogInputDMASupported = ADDIDATA_ENABLE;
	}

	// CASE OF THE xPCIe-3521
	if ((pdev->device == xpcie3521_8_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3521_4_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3521_8C_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3521_4C_BOARD_DEVICE_ID))
	{
		uint8_t b_TCWCounter = 0;
		print_capability(pdev, "xPCIe-3521");

		for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter++)
			data->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter] = 0;

		data->dw_TimerCounterWatchdogOffsetAdress[0] = 128;
		data->b_TimerCounterWatchdogConfigurableAsTimer[0] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[0] = ADDIDATA_DISABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[0] = ADDIDATA_DISABLE;

		data->dw_TimerCounterWatchdogOffsetAdress[1] = 160;
		data->b_TimerCounterWatchdogConfigurableAsTimer[1] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[1] = ADDIDATA_DISABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[1] = ADDIDATA_ENABLE;

		data->b_DigitalInOutputSupported = ADDIDATA_ENABLE;
		data->dw_DigitalInputOffsetAdress = 32;
		data->dw_DigitalOutputOffsetAdress = 48;

		data->b_TTLSupported = ADDIDATA_DISABLE;
		data->dw_TTLOffsetAdress = 0;
		data->b_TTLConfiguration = 0;

		switch (pdev->device)
		{
			case xpcie3521_8_BOARD_DEVICE_ID:
				data->b_NumberOfAnalogInput = 0;
				data->b_NumberOfAnalogOutput = 8;
				break;
			case xpcie3521_4_BOARD_DEVICE_ID:
				data->b_NumberOfAnalogInput = 0;
				data->b_NumberOfAnalogOutput = 4;
				break;
			case xpcie3521_8C_BOARD_DEVICE_ID:
				data->b_NumberOfAnalogInput = 0;
				data->b_NumberOfAnalogOutput = 8;
				break;
			case xpcie3521_4C_BOARD_DEVICE_ID:
				data->b_NumberOfAnalogInput = 0;
				data->b_NumberOfAnalogOutput = 4;
				break;
		}

		data->dw_AnalogOutputOffsetAdress = 96;
		data->b_NumberOfAnalogOutputVoltageMode = 2;
		data->b_AnalogOutputInitialisedVoltageMode = 0;

		data->dw_AnalogInputOffsetAdress = 0;
		data->b_AnalogInputSeDiffConfigurationSupported = ADDIDATA_ENABLE;
		data->b_AnalogInputSeDiffConfiguration = ADDIDATA_SINGLE;

		data->dw_MaxConvertTimeForNanoSecond  = 0;
		data->dw_MinConvertTimeForNanoSecond  = 0;
		data->dw_MaxConvertTimeForMicroSecond = 65535;
		data->dw_MinConvertTimeForMicroSecond = 10;
		data->dw_MaxConvertTimeForMilliSecond = 65535;
		data->dw_MinConvertTimeForMilliSecond = 1;

		data->dw_MaxDelayTimeForMicroSecond = 65535;
		data->dw_MinDelayTimeForMicroSecond = 10;
		data->dw_MaxDelayTimeForMilliSecond = 65535;
		data->dw_MinDelayTimeForMilliSecond = 1;
		data->dw_MaxDelayTimeForSecond  = 0;
		data->dw_MinDelayTimeForSecond  = 0;

		data->b_AnalogInputDMASupported = ADDIDATA_ENABLE;
	}

	// CASE OF THE xPCIe-3121
	if ((pdev->device == xpcie3121_16_8_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3121_16_4_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3121_8_8_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3121_8_4_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3121_16_8C_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3121_16_4C_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3121_8_8C_BOARD_DEVICE_ID) ||
		(pdev->device == xpcie3121_8_4C_BOARD_DEVICE_ID))
	{
		uint8_t b_TCWCounter = 0;
		print_capability(pdev, "xPCIe-3121");

		for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter++)
			data->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter] = 0;

		data->dw_TimerCounterWatchdogOffsetAdress[0] = 128;
		data->b_TimerCounterWatchdogConfigurableAsTimer[0] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[0] = ADDIDATA_DISABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[0] = ADDIDATA_DISABLE;

		data->dw_TimerCounterWatchdogOffsetAdress[1] = 160;
		data->b_TimerCounterWatchdogConfigurableAsTimer[1] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[1] = ADDIDATA_DISABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[1] = ADDIDATA_ENABLE;

		data->b_DigitalInOutputSupported = ADDIDATA_ENABLE;
		data->dw_DigitalInputOffsetAdress = 32;
		data->dw_DigitalOutputOffsetAdress = 48;

		data->b_TTLSupported = ADDIDATA_DISABLE;
		data->dw_TTLOffsetAdress = 0;
		data->b_TTLConfiguration = 0;

		switch (pdev->device)
		{
			case xpcie3121_16_8_BOARD_DEVICE_ID:
				data->b_NumberOfAnalogInput = 16;
				data->b_NumberOfAnalogOutput = 8;
				break;
			case xpcie3121_16_4_BOARD_DEVICE_ID:
				data->b_NumberOfAnalogInput = 16;
				data->b_NumberOfAnalogOutput = 4;
				break;
			case xpcie3121_8_8_BOARD_DEVICE_ID:
				data->b_NumberOfAnalogInput = 8;
				data->b_NumberOfAnalogOutput = 8;
				break;
			case xpcie3121_8_4_BOARD_DEVICE_ID:
				data->b_NumberOfAnalogInput = 8;
				data->b_NumberOfAnalogOutput = 4;
				break;
			case xpcie3121_16_8C_BOARD_DEVICE_ID:
				data->b_NumberOfAnalogInput = 16;
				data->b_NumberOfAnalogOutput = 8;
				break;
			case xpcie3121_16_4C_BOARD_DEVICE_ID:
				data->b_NumberOfAnalogInput = 16;
				data->b_NumberOfAnalogOutput = 4;
				break;
			case xpcie3121_8_8C_BOARD_DEVICE_ID:
				data->b_NumberOfAnalogInput = 8;
				data->b_NumberOfAnalogOutput = 8;
				break;
			case xpcie3121_8_4C_BOARD_DEVICE_ID:
				data->b_NumberOfAnalogInput = 8;
				data->b_NumberOfAnalogOutput = 4;
				break;
		}

		data->dw_AnalogOutputOffsetAdress = 96;
		data->b_NumberOfAnalogOutputVoltageMode = 2;
		data->b_AnalogOutputInitialisedVoltageMode = 0;

		data->dw_AnalogInputOffsetAdress = 0;
		data->b_AnalogInputSeDiffConfigurationSupported = ADDIDATA_ENABLE;
		data->b_AnalogInputSeDiffConfiguration = ADDIDATA_SINGLE;

		data->dw_MaxConvertTimeForNanoSecond  = 0;
		data->dw_MinConvertTimeForNanoSecond  = 0;
		data->dw_MaxConvertTimeForMicroSecond = 65535;
		data->dw_MinConvertTimeForMicroSecond = 10;
		data->dw_MaxConvertTimeForMilliSecond = 65535;
		data->dw_MinConvertTimeForMilliSecond = 1;

		data->dw_MaxDelayTimeForMicroSecond = 65535;
		data->dw_MinDelayTimeForMicroSecond = 10;
		data->dw_MaxDelayTimeForMilliSecond = 65535;
		data->dw_MinDelayTimeForMilliSecond = 1;
		data->dw_MaxDelayTimeForSecond  = 0;
		data->dw_MinDelayTimeForSecond  = 0;

		data->b_AnalogInputDMASupported = ADDIDATA_ENABLE;
	}

	// CASE OF THE xPCI-3002 and xPCI-3003
	if ((pdev->device == xpci3002_16_BOARD_DEVICE_ID) || (pdev->device == xpci3002_8_BOARD_DEVICE_ID) || (pdev->device == xpci3002_4_BOARD_DEVICE_ID) ||
			(pdev->device == xpci3003_BOARD_DEVICE_ID))
	{
		uint8_t b_TCWCounter = 0;
		print_capability(pdev, "xPCI-3002/xPCI-3003");

		for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter++)
			data->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter] = 0;

		data->dw_TimerCounterWatchdogOffsetAdress[0] = 64;

		data->b_TimerCounterWatchdogConfigurableAsTimer[0] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[0] = ADDIDATA_DISABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[0] = ADDIDATA_DISABLE;

		data->b_TimerCounterWatchdogConfigurableAsTimer[1] = ADDIDATA_DISABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[1] = ADDIDATA_DISABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[1] = ADDIDATA_DISABLE;

		data->b_TimerCounterWatchdogConfigurableAsTimer[2] = ADDIDATA_DISABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[2] = ADDIDATA_DISABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[2] = ADDIDATA_DISABLE;

		data->b_DigitalInOutputSupported = ADDIDATA_ENABLE;
		data->dw_DigitalInputOffsetAdress = 32;
		data->dw_DigitalOutputOffsetAdress = 48;

		data->b_TTLSupported = ADDIDATA_DISABLE;
		data->dw_TTLOffsetAdress = 0;
		data->b_TTLConfiguration = 0;

		if (pdev->device == xpci3002_16_BOARD_DEVICE_ID)
			data->b_NumberOfAnalogInput = 16; // for the xpci3002-16
		if (pdev->device == xpci3002_8_BOARD_DEVICE_ID)
			data->b_NumberOfAnalogInput = 8; // for the xpci3002-8
		if ((pdev->device == xpci3002_4_BOARD_DEVICE_ID) || (pdev->device == xpci3003_BOARD_DEVICE_ID))
			data->b_NumberOfAnalogInput = 4; // for the xpci3002-4 and xpci3003

		data->b_AnalogInputSeDiffConfigurationSupported = ADDIDATA_DISABLE; /* AnalogInput Single/Differential not supported */
		data->b_AnalogInputSeDiffConfiguration = ADDIDATA_DIFFERENTIAL;

		data->dw_AnalogInputOffsetAdress = 0;
		if (pdev->device != xpci3003_BOARD_DEVICE_ID)
		{
			data->dw_MaxConvertTimeForNanoSecond  = 0;
			data->dw_MinConvertTimeForNanoSecond  = 0;
			data->dw_MaxConvertTimeForMicroSecond = 65535; // 16 bit resolution
			data->dw_MinConvertTimeForMicroSecond = 5;
			data->dw_MaxConvertTimeForMilliSecond = 65535; // 16 bit resolution
			data->dw_MinConvertTimeForMilliSecond = 1;
		}
		else
		{
			data->dw_MaxConvertTimeForNanoSecond  = 65535; // 16 bit resolution
			data->dw_MinConvertTimeForNanoSecond  = 2500;
			data->dw_MaxConvertTimeForMicroSecond = 65535; // 16 bit resolution
			data->dw_MinConvertTimeForMicroSecond = 3;
			data->dw_MaxConvertTimeForMilliSecond = 65535; // 16 bit resolution
			data->dw_MinConvertTimeForMilliSecond = 1;
		}

		data->dw_MaxDelayTimeForMicroSecond = 65535; // 16 bit resolution
		data->dw_MinDelayTimeForMicroSecond = 1;
		data->dw_MaxDelayTimeForMilliSecond = 65535; // 16 bit resolution
		data->dw_MinDelayTimeForMilliSecond = 1;
		data->dw_MaxDelayTimeForSecond  = 0;
		data->dw_MinDelayTimeForSecond  = 0;

		data->b_AnalogInputDMASupported = ADDIDATA_ENABLE;

		data->b_NumberOfAnalogOutput = 0;
		data->dw_AnalogOutputOffsetAdress = 0;
		data->b_NumberOfAnalogOutputVoltageMode = 0;
		data->b_AnalogOutputInitialisedVoltageMode = 0;
	}

	// CASE OF THE xPCI-3002 Audi
	if (pdev->device == xpci3002_16_AUDI_BOARD_DEVICE_ID)
	{

		uint8_t b_TCWCounter = 0;
		print_capability(pdev, "xPCI-3002 Audi");
		for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter++)
			data->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter] = 0;

		data->dw_TimerCounterWatchdogOffsetAdress[0] = 64;
		data->dw_TimerCounterWatchdogOffsetAdress[1] = 96;
		data->dw_TimerCounterWatchdogOffsetAdress[2] = 128;

		data->b_TimerCounterWatchdogConfigurableAsTimer[0] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[0] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[0] = ADDIDATA_DISABLE;

		data->b_TimerCounterWatchdogConfigurableAsTimer[1] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[1] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[1] = ADDIDATA_DISABLE;

		data->b_TimerCounterWatchdogConfigurableAsTimer[2] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[2] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[2] = ADDIDATA_DISABLE;

		data->b_DigitalInOutputSupported = ADDIDATA_ENABLE;
		data->dw_DigitalInputOffsetAdress = 32;
		data->dw_DigitalOutputOffsetAdress = 48;

		data->b_TTLSupported = ADDIDATA_DISABLE;
		data->dw_TTLOffsetAdress = 0;
		data->b_TTLConfiguration = 0;

		if (pdev->device == xpci3002_16_AUDI_BOARD_DEVICE_ID)
			data->b_NumberOfAnalogInput = 16; // for the xpci3002-16

		data->b_AnalogInputSeDiffConfigurationSupported = ADDIDATA_DISABLE;
		data->b_AnalogInputSeDiffConfiguration = ADDIDATA_DIFFERENTIAL;

		data->dw_AnalogInputOffsetAdress = 0;
		data->dw_MaxConvertTimeForNanoSecond  = 0;
		data->dw_MinConvertTimeForNanoSecond  = 0;
		data->dw_MaxConvertTimeForMicroSecond = 65535; // 16 bit resolution
		data->dw_MinConvertTimeForMicroSecond = 5;
		data->dw_MaxConvertTimeForMilliSecond = 65535; // 16 bit resolution
		data->dw_MinConvertTimeForMilliSecond = 1;

		data->dw_MaxDelayTimeForMicroSecond = 65535; // 16 bit resolution
		data->dw_MinDelayTimeForMicroSecond = 1;
		data->dw_MaxDelayTimeForMilliSecond = 65535; // 16 bit resolution
		data->dw_MinDelayTimeForMilliSecond = 1;
		data->dw_MaxDelayTimeForSecond  = 0;
		data->dw_MinDelayTimeForSecond  = 0;

		data->b_AnalogInputDMASupported = ADDIDATA_ENABLE;

		data->b_NumberOfAnalogOutput = 0;
		data->dw_AnalogOutputOffsetAdress = 0;
		data->b_NumberOfAnalogOutputVoltageMode = 0;
		data->b_AnalogOutputInitialisedVoltageMode = 0;
	}

	// CASE OF THE xPCI-3116 and xPCI-3110
	if ((pdev->device == xpci3116_16_BOARD_DEVICE_ID) || (pdev->device == xpci3116_8_BOARD_DEVICE_ID) ||
			(pdev->device == xpci3110_16_BOARD_DEVICE_ID) || (pdev->device == xpci3110_8_BOARD_DEVICE_ID))
	{

		uint8_t b_TCWCounter = 0;
		print_capability(pdev, "xPCI-3116/xPCI-3110");
		for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter++)
			data->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter] = 128 + (b_TCWCounter * 32);

		data->b_TimerCounterWatchdogConfigurableAsTimer[0] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[0] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[0] = ADDIDATA_DISABLE;

		data->b_TimerCounterWatchdogConfigurableAsTimer[1] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[1] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[1] = ADDIDATA_ENABLE; // analog

		data->b_TimerCounterWatchdogConfigurableAsTimer[2] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[2] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[2] = ADDIDATA_ENABLE; // digital

		data->b_DigitalInOutputSupported = ADDIDATA_ENABLE;
		data->dw_DigitalInputOffsetAdress = 32;
		data->dw_DigitalOutputOffsetAdress = 48;

		data->b_TTLSupported = ADDIDATA_ENABLE;
		data->dw_TTLOffsetAdress = 64;
		data->b_TTLConfiguration = 0;

		if ((pdev->device == xpci3116_16_BOARD_DEVICE_ID) || (pdev->device == xpci3110_16_BOARD_DEVICE_ID))
			data->b_NumberOfAnalogInput = 16; // for the xpci3116-16 and xpci3110-16
		if ((pdev->device == xpci3116_8_BOARD_DEVICE_ID) || (pdev->device == xpci3110_8_BOARD_DEVICE_ID))
			data->b_NumberOfAnalogInput = 8; // for the xpci3116-8 and xpci3110-8

		data->b_AnalogInputSeDiffConfigurationSupported = ADDIDATA_ENABLE;
		data->b_AnalogInputSeDiffConfiguration = ADDIDATA_SINGLE;

		data->dw_AnalogInputOffsetAdress = 0;
		data->dw_MaxConvertTimeForNanoSecond  = 0;
		data->dw_MinConvertTimeForNanoSecond  = 0;
		data->dw_MaxConvertTimeForMicroSecond = 65535; // 16 bit resolution
		data->dw_MinConvertTimeForMicroSecond = 5;
		data->dw_MaxConvertTimeForMilliSecond = 65535; // 16 bit resolution
		data->dw_MinConvertTimeForMilliSecond = 1;

		data->dw_MaxDelayTimeForMicroSecond = 65535; // 16 bit resolution
		data->dw_MinDelayTimeForMicroSecond = 1;
		data->dw_MaxDelayTimeForMilliSecond = 65535; // 16 bit resolution
		data->dw_MinDelayTimeForMilliSecond = 1;
		data->dw_MaxDelayTimeForSecond  = 0;
		data->dw_MinDelayTimeForSecond  = 0;

		data->b_AnalogInputDMASupported = ADDIDATA_ENABLE;

		data->b_NumberOfAnalogOutput = 4;
		data->dw_AnalogOutputOffsetAdress = 96;
		data->b_NumberOfAnalogOutputVoltageMode = 2; // Bipolar 12 bit, Unipolar 11 bit
		data->b_AnalogOutputInitialisedVoltageMode = 0;
	}

	// CASE OF THE xPCI-3016 and xPCI-3010
	if ((pdev->device == xpci3016_16_BOARD_DEVICE_ID) || (pdev->device == xpci3016_8_BOARD_DEVICE_ID) || (pdev->device == xpci3016_4_BOARD_DEVICE_ID) ||
			(pdev->device == xpci3010_16_BOARD_DEVICE_ID) || (pdev->device == xpci3010_8_BOARD_DEVICE_ID) || (pdev->device == xpci3010_4_BOARD_DEVICE_ID))
	{

		uint8_t b_TCWCounter = 0;
		print_capability(pdev, "xPCI-3116/xPCI-3110");
		for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter++)
			data->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter] = 128 + (b_TCWCounter * 32);

		data->b_TimerCounterWatchdogConfigurableAsTimer[0] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[0] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[0] = ADDIDATA_DISABLE;

		data->b_TimerCounterWatchdogConfigurableAsTimer[1] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[1] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[1] = ADDIDATA_DISABLE; // analog

		data->b_TimerCounterWatchdogConfigurableAsTimer[2] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[2] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[2] = ADDIDATA_ENABLE; // digital

		data->b_DigitalInOutputSupported = ADDIDATA_ENABLE;
		data->dw_DigitalInputOffsetAdress = 32;
		data->dw_DigitalOutputOffsetAdress = 48;

		data->b_TTLSupported = ADDIDATA_ENABLE;
		data->dw_TTLOffsetAdress = 64;
		data->b_TTLConfiguration = 0;

		if ((pdev->device == xpci3016_16_BOARD_DEVICE_ID) || (pdev->device == xpci3010_16_BOARD_DEVICE_ID))
			data->b_NumberOfAnalogInput = 16; // for the xpci3016-16 and xpci3010-16
		if ((pdev->device == xpci3016_8_BOARD_DEVICE_ID) || (pdev->device == xpci3010_8_BOARD_DEVICE_ID))
			data->b_NumberOfAnalogInput = 8; // for the xpci3016-8 and xpci3010-8
		if ((pdev->device == xpci3016_4_BOARD_DEVICE_ID) || (pdev->device == xpci3010_4_BOARD_DEVICE_ID))
			data->b_NumberOfAnalogInput = 4; // for the xpci3016-4 and xpci3010-4

		data->b_AnalogInputSeDiffConfigurationSupported = ADDIDATA_ENABLE;
		data->b_AnalogInputSeDiffConfiguration = ADDIDATA_SINGLE;

		data->dw_AnalogInputOffsetAdress = 0;
		data->dw_MaxConvertTimeForNanoSecond  = 0;
		data->dw_MinConvertTimeForNanoSecond  = 0;
		data->dw_MaxConvertTimeForMicroSecond = 65535; // 16 bit resolution
		data->dw_MinConvertTimeForMicroSecond = 5;
		data->dw_MaxConvertTimeForMilliSecond = 65535; // 16 bit resolution
		data->dw_MinConvertTimeForMilliSecond = 1;

		data->dw_MaxDelayTimeForMicroSecond = 65535; // 16 bit resolution
		data->dw_MinDelayTimeForMicroSecond = 1;
		data->dw_MaxDelayTimeForMilliSecond = 65535; // 16 bit resolution
		data->dw_MinDelayTimeForMilliSecond = 1;
		data->dw_MaxDelayTimeForSecond  = 0;
		data->dw_MinDelayTimeForSecond  = 0;

		data->b_AnalogInputDMASupported = ADDIDATA_ENABLE;

		data->b_NumberOfAnalogOutput = 0;
		data->dw_AnalogOutputOffsetAdress = 0;
		data->b_NumberOfAnalogOutputVoltageMode = 0;
		data->b_AnalogOutputInitialisedVoltageMode = 0;
	}

	// CASE OF THE xPCI-3106 and xPCI-3100
	if ((pdev->device == xpci3106_16_BOARD_DEVICE_ID) || (pdev->device == xpci3106_8_BOARD_DEVICE_ID) ||
			(pdev->device == xpci3100_16_BOARD_DEVICE_ID) || (pdev->device == xpci3100_8_BOARD_DEVICE_ID))
	{

		uint8_t b_TCWCounter = 0;
		print_capability(pdev, "xPCI-3106/xPCI-3100");
		for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter++)
			data->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter] = 128 + (b_TCWCounter * 32);

		data->b_TimerCounterWatchdogConfigurableAsTimer[0] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[0] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[0] = ADDIDATA_DISABLE;

		data->b_TimerCounterWatchdogConfigurableAsTimer[1] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[1] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[1] = ADDIDATA_DISABLE; // analog

		data->b_TimerCounterWatchdogConfigurableAsTimer[2] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[2] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[2] = ADDIDATA_DISABLE; // digital

		data->b_DigitalInOutputSupported = ADDIDATA_DISABLE;
		data->dw_DigitalInputOffsetAdress = 0;
		data->dw_DigitalOutputOffsetAdress = 0;

		data->b_TTLSupported = ADDIDATA_ENABLE;
		data->dw_TTLOffsetAdress = 64;
		data->b_TTLConfiguration = 0;

		if ((pdev->device == xpci3106_16_BOARD_DEVICE_ID) || (pdev->device == xpci3100_16_BOARD_DEVICE_ID))
			data->b_NumberOfAnalogInput = 16; // for the xpci3106-16 and xpci3100-16
		if ((pdev->device == xpci3106_8_BOARD_DEVICE_ID) || (pdev->device == xpci3100_8_BOARD_DEVICE_ID))
			data->b_NumberOfAnalogInput = 8; // for the xpci3106-8 and xpci3100-8

		data->b_AnalogInputSeDiffConfigurationSupported = ADDIDATA_ENABLE;
		data->b_AnalogInputSeDiffConfiguration = ADDIDATA_SINGLE;

		data->dw_AnalogInputOffsetAdress = 0;
		data->dw_MaxConvertTimeForNanoSecond  = 0;
		data->dw_MinConvertTimeForNanoSecond  = 0;
		data->dw_MaxConvertTimeForMicroSecond = 65535; // 16 bit resolution
		data->dw_MinConvertTimeForMicroSecond = 10;
		data->dw_MaxConvertTimeForMilliSecond = 65535; // 16 bit resolution
		data->dw_MinConvertTimeForMilliSecond = 1;

		data->dw_MaxDelayTimeForMicroSecond = 65535; // 16 bit resolution
		data->dw_MinDelayTimeForMicroSecond = 1;
		data->dw_MaxDelayTimeForMilliSecond = 65535; // 16 bit resolution
		data->dw_MinDelayTimeForMilliSecond = 1;
		data->dw_MaxDelayTimeForSecond  = 0;
		data->dw_MinDelayTimeForSecond  = 0;

		data->b_AnalogInputDMASupported = ADDIDATA_DISABLE;

		data->b_NumberOfAnalogOutput = 4;
		data->dw_AnalogOutputOffsetAdress = 96;
		data->b_NumberOfAnalogOutputVoltageMode = 2; // Bipolar 12 bit, Unipolar 11 bit
		data->b_AnalogOutputInitialisedVoltageMode = 0;
	}

	// CASE OF THE xPCI-3006 and xPCI-3000
	if ((pdev->device == xpci3006_16_BOARD_DEVICE_ID) || (pdev->device == xpci3006_8_BOARD_DEVICE_ID) || (pdev->device == xpci3006_4_BOARD_DEVICE_ID) ||
			(pdev->device == xpci3000_16_BOARD_DEVICE_ID) || (pdev->device == xpci3000_8_BOARD_DEVICE_ID) || (pdev->device == xpci3000_4_BOARD_DEVICE_ID))
	{

		uint8_t b_TCWCounter = 0;
		print_capability(pdev, "xPCI-3006/xPCI-3000");
		for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter++)
			data->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter] = 128 + (b_TCWCounter * 32);

		data->b_TimerCounterWatchdogConfigurableAsTimer[0] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[0] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[0] = ADDIDATA_DISABLE;

		data->b_TimerCounterWatchdogConfigurableAsTimer[1] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[1] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[1] = ADDIDATA_DISABLE; // analog

		data->b_TimerCounterWatchdogConfigurableAsTimer[2] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[2] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[2] = ADDIDATA_DISABLE; // digital

		data->b_DigitalInOutputSupported = ADDIDATA_DISABLE;
		data->dw_DigitalInputOffsetAdress = 0;
		data->dw_DigitalOutputOffsetAdress = 0;

		data->b_TTLSupported = ADDIDATA_ENABLE;
		data->dw_TTLOffsetAdress = 64;
		data->b_TTLConfiguration = 0;

		if ((pdev->device == xpci3006_16_BOARD_DEVICE_ID) || (pdev->device == xpci3000_16_BOARD_DEVICE_ID))
			data->b_NumberOfAnalogInput = 16; // for the xpci3006-16 and xpci3000-16
		if ((pdev->device == xpci3006_8_BOARD_DEVICE_ID) || (pdev->device == xpci3000_8_BOARD_DEVICE_ID))
			data->b_NumberOfAnalogInput = 8; // for the xpci3006-8 and xpci3000-8
		if ((pdev->device == xpci3006_4_BOARD_DEVICE_ID) || (pdev->device == xpci3000_4_BOARD_DEVICE_ID))
			data->b_NumberOfAnalogInput = 4; // for the xpci3006-4 and xpci3000-4

		data->b_AnalogInputSeDiffConfigurationSupported = ADDIDATA_ENABLE; /* if ADDIDATA_ENABLE, b_AnalogInputSeDiffConfiguration can be set. otherwise b_AnalogInputSeDiffConfiguration is initialized once for all */
		data->b_AnalogInputSeDiffConfiguration = ADDIDATA_SINGLE;

		data->dw_AnalogInputOffsetAdress = 0;
		data->dw_MaxConvertTimeForNanoSecond  = 0;
		data->dw_MinConvertTimeForNanoSecond  = 0;
		data->dw_MaxConvertTimeForMicroSecond = 65535; // 16 bit resolution
		data->dw_MinConvertTimeForMicroSecond = 10;
		data->dw_MaxConvertTimeForMilliSecond = 65535; // 16 bit resolution
		data->dw_MinConvertTimeForMilliSecond = 1;

		data->dw_MaxDelayTimeForMicroSecond = 65535; // 16 bit resolution
		data->dw_MinDelayTimeForMicroSecond = 1;
		data->dw_MaxDelayTimeForMilliSecond = 65535; // 16 bit resolution
		data->dw_MinDelayTimeForMilliSecond = 1;
		data->dw_MaxDelayTimeForSecond  = 0;
		data->dw_MinDelayTimeForSecond  = 0;

		data->b_AnalogInputDMASupported = ADDIDATA_DISABLE;

		data->b_NumberOfAnalogOutput = 0;
		data->dw_AnalogOutputOffsetAdress = 0;
		data->b_NumberOfAnalogOutputVoltageMode = 0;
		data->b_AnalogOutputInitialisedVoltageMode = 0;
	}

	// CASE OF THE xPCI-3500
	if (pdev->device == xpci3500_BOARD_DEVICE_ID)
	{

		uint8_t b_TCWCounter = 0;
		print_capability(pdev, "xPCI-3500");
		for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter++)
			data->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter] = 128 + (b_TCWCounter * 32);

		data->b_TimerCounterWatchdogConfigurableAsTimer[0] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[0] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[0] = ADDIDATA_DISABLE;

		data->b_TimerCounterWatchdogConfigurableAsTimer[1] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[1] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[1] = ADDIDATA_DISABLE; // analog

		data->b_TimerCounterWatchdogConfigurableAsTimer[2] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[2] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[2] = ADDIDATA_DISABLE; // digital

		data->b_DigitalInOutputSupported = ADDIDATA_DISABLE;
		data->dw_DigitalInputOffsetAdress = 0;
		data->dw_DigitalOutputOffsetAdress = 0;

		data->b_TTLSupported = ADDIDATA_ENABLE;
		data->dw_TTLOffsetAdress = 64;
		data->b_TTLConfiguration = 0;

		data->b_NumberOfAnalogInput = 0;
		data->b_AnalogInputSeDiffConfigurationSupported = 0;
		data->b_AnalogInputSeDiffConfiguration = 0;
		data->dw_AnalogInputOffsetAdress = 0;
		data->dw_MaxConvertTimeForNanoSecond = 0;
		data->dw_MinConvertTimeForNanoSecond = 0;
		data->dw_MaxConvertTimeForMicroSecond = 0;
		data->dw_MinConvertTimeForMicroSecond = 0;
		data->dw_MaxConvertTimeForMilliSecond = 0;
		data->dw_MinConvertTimeForMilliSecond = 0;

		data->dw_MaxDelayTimeForMicroSecond = 0;
		data->dw_MinDelayTimeForMicroSecond = 0;
		data->dw_MaxDelayTimeForMilliSecond = 0;
		data->dw_MinDelayTimeForMilliSecond = 0;
		data->dw_MaxDelayTimeForSecond  = 0;
		data->dw_MinDelayTimeForSecond  = 0;

		data->b_AnalogInputDMASupported = ADDIDATA_DISABLE;

		data->b_NumberOfAnalogOutput = 4;
		data->dw_AnalogOutputOffsetAdress = 96;
		data->b_NumberOfAnalogOutputVoltageMode = 2; // Bipolar 12 bit, Unipolar 11 bit
		data->b_AnalogOutputInitialisedVoltageMode = 0;
	}

	// CASE OF THE xPCI-3501
	if (pdev->device == xpci3501_BOARD_DEVICE_ID)
	{

		uint8_t b_TCWCounter = 0;
		print_capability(pdev, "xPCI-3501");
		for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter++)
			data->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter] = 0x20 + (b_TCWCounter * 32);

		data->b_TimerCounterWatchdogConfigurableAsTimer[0] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[0] = ADDIDATA_DISABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[0] = ADDIDATA_ENABLE;

		data->b_DigitalInOutputSupported = ADDIDATA_ENABLE;
		data->dw_DigitalInputOffsetAdress = 0x50;
		data->dw_DigitalOutputOffsetAdress = 0x40;

		data->b_TTLSupported = ADDIDATA_DISABLE;
		data->dw_TTLOffsetAdress = 0;
		data->b_TTLConfiguration = 0;

		data->b_NumberOfAnalogInput = 0;
		data->b_AnalogInputSeDiffConfigurationSupported = 0;
		data->b_AnalogInputSeDiffConfiguration = 0;
		data->dw_AnalogInputOffsetAdress = 0;
		data->dw_MaxConvertTimeForNanoSecond = 0;
		data->dw_MinConvertTimeForNanoSecond = 0;
		data->dw_MaxConvertTimeForMicroSecond = 0;
		data->dw_MinConvertTimeForMicroSecond = 0;
		data->dw_MaxConvertTimeForMilliSecond = 0;
		data->dw_MinConvertTimeForMilliSecond = 0;

		data->dw_MaxDelayTimeForMicroSecond = 0;
		data->dw_MinDelayTimeForMicroSecond = 0;
		data->dw_MaxDelayTimeForMilliSecond = 0;
		data->dw_MinDelayTimeForMilliSecond = 0;
		data->dw_MaxDelayTimeForSecond  = 0;
		data->dw_MinDelayTimeForSecond  = 0;

		data->b_AnalogInputDMASupported = ADDIDATA_DISABLE;

#ifdef USE_ADDIEEPROM
		/* Read the number of channels directly from the board header */
		i_xpci3xxx_ReadEepromHeader_NbrOfOutputs (pdev);
#else
		data->b_NumberOfAnalogOutput = 8;
#endif
		data->dw_AnalogOutputOffsetAdress = 0x0;
		data->b_NumberOfAnalogOutputVoltageMode = 2; // Bipolar 14 bit, Unipolar 13 bit
		data->b_AnalogOutputInitialisedVoltageMode = 0;
	}

	// CASE OF THE xPCI3009
	if ( pdev->device == xpci3009_BOARD_DEVICE_ID )
	{
		print_capability(pdev, "xPCI-3009");
		//for (b_TCWCounter = 0 ; b_TCWCounter < ADDIDATA_MAX_TCW ; b_TCWCounter++)
		//	data->dw_TimerCounterWatchdogOffsetAdress[b_TCWCounter] = 128 + (b_TCWCounter * 32);

		data->b_TimerCounterWatchdogConfigurableAsTimer[0] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[0] = ADDIDATA_ENABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[0] = ADDIDATA_DISABLE;

		data->b_TimerCounterWatchdogConfigurableAsTimer[1] = ADDIDATA_DISABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[1] = ADDIDATA_DISABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[1] = ADDIDATA_DISABLE;

		data->b_TimerCounterWatchdogConfigurableAsTimer[2] = ADDIDATA_DISABLE;
		data->b_TimerCounterWatchdogConfigurableAsCounter[2] =ADDIDATA_DISABLE;
		data->b_TimerCounterWatchdogConfigurableAsWatchdog[2] = ADDIDATA_DISABLE;

		data->b_DigitalInOutputSupported = ADDIDATA_ENABLE;
		data->dw_DigitalInputOffsetAdress = 32; // Ok, look in the io mapping
		data->dw_DigitalOutputOffsetAdress = 48;

		data->b_TTLSupported = ADDIDATA_DISABLE;
		data->dw_TTLOffsetAdress = 0;
		data->b_TTLConfiguration = 0;

		data->b_NumberOfAnalogInput = 16;

		data->b_AnalogInputSeDiffConfigurationSupported = ADDIDATA_ENABLE;
		data->b_AnalogInputSeDiffConfiguration = ADDIDATA_SINGLE;

		data->dw_AnalogInputOffsetAdress = 0;
		data->dw_MaxConvertTimeForNanoSecond  = 0;
		data->dw_MinConvertTimeForNanoSecond  = 0;
		data->dw_MaxConvertTimeForMicroSecond = 65535; // 16 bit resolution
		data->dw_MinConvertTimeForMicroSecond = 5;
		data->dw_MaxConvertTimeForMilliSecond = 65535; // 16 bit resolution
		data->dw_MinConvertTimeForMilliSecond = 1;

		data->dw_MaxDelayTimeForMicroSecond = 65535; // 16 bit resolution
		data->dw_MinDelayTimeForMicroSecond = 1;
		data->dw_MaxDelayTimeForMilliSecond = 65535; // 16 bit resolution
		data->dw_MinDelayTimeForMilliSecond = 1;
		data->dw_MaxDelayTimeForSecond  = 0;
		data->dw_MinDelayTimeForSecond  = 0;

		data->b_AnalogInputDMASupported = ADDIDATA_ENABLE;

		data->b_NumberOfAnalogOutput = 4;
		data->dw_AnalogOutputOffsetAdress = 96;
		data->b_NumberOfAnalogOutputVoltageMode = 2; // Bipolar 12 bit, Unipolar 11 bit
		data->b_AnalogOutputInitialisedVoltageMode = 0;
	}

	init_waitqueue_head (&data->wq);
}


/** return the private data field of a pci_dev structure.
* @note The implementation of this function differs in 2.4 and 2.6 kernel
*/
static __inline__ struct xpci3xxx_str_BoardInformations * XPCI3XXX_PRIVDATA(struct pci_dev * pdev)
{
	return (struct xpci3xxx_str_BoardInformations *) pci_get_drvdata(pdev);
}

/** lock the board */
static __inline__ void XPCI3XXX_LOCK(struct pci_dev * pdev, unsigned long * flags)
{
	spin_lock_irqsave(& (XPCI3XXX_PRIVDATA(pdev)->lock) , *flags );
}

/** unlock the board */
static __inline__ void XPCI3XXX_UNLOCK(struct pci_dev * pdev, unsigned long flags)
{
	spin_unlock_irqrestore(& (XPCI3XXX_PRIVDATA(pdev)->lock) , flags );
}

// append to the list of known devices
extern void xpci3xxx_known_dev_append(struct pci_dev * pdev);
// remove from the list of known devices
extern void xpci3xxx_known_dev_remove(struct pci_dev * pdev);

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,27)
#undef __user
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#undef __iomem
#endif

#ifdef CONFIG_XPCI3XXX_DEBUG
#warning Compiled with debug !
#define XPCI3XXX_DEBUG(args...) printk(KERN_DEBUG ## args)
#define XPCI3XXX_DEBUG_FCN() XPCI3XXX_DEBUG("%s\n",__FUNCTION__);
#define XPCI3XXX_DEBUG_INTARRAY(__name,__from, __to,__array) { int i; for(i=__from;i<__to;i++) XPCI3XXX_DEBUG("%s %d: %d\n",__name, i, __array[i]); }
#define XPCI3XXX_DEBUG_CALLRET(__call...) printk(KERN_DEBUG "%s returned %d\n", #__call, __call)
#else
#define XPCI3XXX_DEBUG(args...)
#define XPCI3XXX_DEBUG_FCN()
#define XPCI3XXX_DEBUG_INTARRAY(__name,__from, __to,__array)
#define XPCI3XXX_DEBUG_CALLRET(__call...) __call
#endif

#endif // __XPCI3XXX_PRIVDATA_H_
