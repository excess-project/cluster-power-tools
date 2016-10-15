/** @file primary_ao.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
*
* @version  $LastChangedRevision$
* @version $LastChangedDate$
*
* Primary functions (hardware access) for the analog output functionality of the board.
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

EXPORT_SYMBOL(i_xpci3xxx_InitAnalogOutput);
EXPORT_SYMBOL(i_xpci3xxx_ReadAnalogOutputBit);
EXPORT_SYMBOL(i_xpci3xxx_WriteAnalogOutputValue);
EXPORT_SYMBOL(i_xpci3xxx_GetNumberOfAnalogOutputChannels);
// EXPORT_SYMBOL(i_xpci3xxx_FastReadAnalogOutputBit);
EXPORT_SYMBOL(i_xpci3xxx_FastWriteAnalogOutputValue);

/** Get the number of analog output channels.
 *
 * @WARNING:	To get the real number of channels of the APCI-3501<br>
 * 				you have to edit _makefile:<br>
 * 				echo "//#define USE_ADDIEEPROM	1" >> xpci3xxx-options.h; \
 * 				and remove // in the previous line.
 * 				This add functions to read the board header.
 * 				Once USE_ADDIEEPROM is defined you have to load
 * 				AddiEEPROM module before the xpci3xxx module.
 *
 * @param [in] pdev				: The device to use.
 * @param [in] b_NbrChannels	: The number of analog output channels.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 */
int i_xpci3xxx_GetNumberOfAnalogOutputChannels (struct pci_dev *pdev, uint8_t *b_NbrChannels)
{
	if (!pdev) return 1;

	/* If no output channels */
	if (!XPCI3XXX_PRIVDATA(pdev)->b_NumberOfAnalogOutput) {
		*b_NbrChannels = 0;
		return 0;
	}

	*b_NbrChannels = XPCI3XXX_PRIVDATA(pdev)->b_NumberOfAnalogOutput;

	return 0;
}

/**
 *
 * Initialises one analog output channel
 *
 * @param[in]  pdev : PCI Device struct                \n
 * @param[in]   b_Channel      : channel number of the output     \n
 *                                              to be initialise                 \n
 * @param[in]   b_VoltageMode  : Voltage mode of the analog output\n
 *                                              ADDIDATA_BIPOLAR: 0: -10V - +10V \n
 *                                              ADDIDATA_UNIPOLAR:1: 0V - +10V   \n
 *
 *
 * @return   0 : No error                                            \n
 *                       1 : The handle parameter of the board is wrong          \n
 *                       2 : Wrong channel number                                \n
 *                       3 : Wrong voltage mode								     \n
 *                       4 : A conversion is already started                     \n
 */

int i_xpci3xxx_InitAnalogOutput (struct pci_dev * pdev,
                                uint8_t   b_Channel,
                                uint8_t   b_VoltageMode)
{
	uint32_t  dw_CommandRegister = 0;

//	printk("%s:%d\n",__FUNCTION__,__LINE__); // __FILE__,__FUNCTION__,__LINE__

	if (!pdev) return 1;

	/* Tests the channel */
	if (b_Channel >= XPCI3XXX_PRIVDATA(pdev)->b_NumberOfAnalogOutput)
		return 2;

	/* Tests the gain */
	if (b_VoltageMode >= XPCI3XXX_PRIVDATA(pdev)->b_NumberOfAnalogOutputVoltageMode)
		return 3;

	/* Read the EOC Status bit */
	/* EOC is B0 of dw_CommandRegister ? dw_CommandRegister & 0x100 */

	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		INPDW_MEM(GET_MEM_BAR3(pdev), XPCI3XXX_PRIVDATA(pdev)->dw_AnalogOutputOffsetAdress, &dw_CommandRegister);
	else
		INPDW((GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_AnalogOutputOffsetAdress), &dw_CommandRegister);

/*	*pi_ErrorCode = WAIT_READY_BIT    (b_CallLevel,
                                      ps_HardwareInformation,
                                      0,
                                      8,
                                      TIMEOUT_READY_BIT);*/

	/* Returns -2 if the EOC flag is not ready */
	if ( (dw_CommandRegister & 0x100) != 0x100 )
		return 4;

	/* Evaluate the voltage mode */
	dw_CommandRegister = b_VoltageMode & 0x1;

	/* Set The voltage mode */
	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		OUTPDW_MEM(GET_MEM_BAR3(pdev), XPCI3XXX_PRIVDATA(pdev)->dw_AnalogOutputOffsetAdress, dw_CommandRegister);
	else
		OUTPDW((GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_AnalogOutputOffsetAdress), dw_CommandRegister);
/*	OUTP_WAIT_READY   (b_CallLevel,
	                     ps_HardwareInformation,
	                     0,
	                     dw_CommandRegister1,
	                     0,
	                     NULL,
	                     0,
	                     0);*/

	XPCI3XXX_PRIVDATA(pdev)->b_AnalogOutputInitialisedVoltageMode = b_VoltageMode;

	return 0;
}


/**
 *
 * Reads the EOC Bit of the analog output conversion
 *
 * @param[in]  pdev : PCI Device struct               \n
 *
 * @param[out] pb_AnalogOutputConversionFinished:                \n
 *                                   0: no analog output conversion finished    \n
 *                                   1: analog output conversion finished       \n
 *
 * @return   0 : No error                                           \n
 *                       1 : The handle parameter of the board is wrong         \n
 */

int i_xpci3xxx_ReadAnalogOutputBit (struct pci_dev * pdev, uint8_t* pb_AnalogOutputConversionFinished)
{
	uint32_t dw_CommandRegister = 0;

//	printk("%s:%d\n",__FUNCTION__,__LINE__); // __FILE__,__FUNCTION__,__LINE__

	if (!pdev) return 1;

	/* Read the EOC Status bit */
	/* EOC is B0 of dw_CommandRegister ? dw_CommandRegister & 0x100 */
	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		INPDW_MEM(GET_MEM_BAR3(pdev), XPCI3XXX_PRIVDATA(pdev)->dw_AnalogOutputOffsetAdress, &dw_CommandRegister);
	else
		INPDW((GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_AnalogOutputOffsetAdress), &dw_CommandRegister);

/*	*pi_ErrorCode = WAIT_READY_BIT    (b_CallLevel,
                                      ps_HardwareInformation,
                                      0,
                                      8,
                                      TIMEOUT_READY_BIT);*/

	/* Returns -2 if the EOC flag is not ready */
	if ( (dw_CommandRegister & 0x100) == 0x100 )
		*pb_AnalogOutputConversionFinished = 1;
	else
		*pb_AnalogOutputConversionFinished = 0;

	return 0;
}

/**
 *
 * Writes the value of the analog output channel
 *
 * @param[in]  pdev : PCI Device struct              \n
 * @param[in]   b_Channel      : channel number of the output   \n
 *                                              to be initialise               \n
 * @param[in] dw_WriteValue : Value of the analog output       \n
 *                                             channel                         \n
 *
 *
 * @return   0 : No error                                          \n
 *                       1 : The handle parameter of the board is wrong        \n
 *                       2 : Wrong channel number                              \n
 *                       3 : Wrong write value							       \n
 */

int i_xpci3xxx_WriteAnalogOutputValue (struct pci_dev * pdev,
										uint8_t   b_Channel,
										uint32_t dw_WriteValue)
{
	uint32_t dw_CommandRegisterPolarity = 0;
	uint32_t dw_CommandRegister = 0;

	if (!pdev) return 1;

	/* Tests the channel */
	if (b_Channel >= XPCI3XXX_PRIVDATA(pdev)->b_NumberOfAnalogOutput)
		return 2;

	/* Tests the write value */
	switch (pdev->device)
	{
		case xpci3501_BOARD_DEVICE_ID:
		{
			if (((XPCI3XXX_PRIVDATA(pdev)->b_AnalogOutputInitialisedVoltageMode == ADDIDATA_UNIPOLAR) && (dw_WriteValue > 8191)) ||
				((XPCI3XXX_PRIVDATA(pdev)->b_AnalogOutputInitialisedVoltageMode == ADDIDATA_BIPOLAR) && (dw_WriteValue > 16383)))
				return 4;
		}
		break;

		case xpcie3121_16_8_BOARD_DEVICE_ID:
		case xpcie3121_16_4_BOARD_DEVICE_ID:
		case xpcie3121_8_8_BOARD_DEVICE_ID:
		case xpcie3121_8_4_BOARD_DEVICE_ID:
		case xpcie3121_16_8C_BOARD_DEVICE_ID:
		case xpcie3121_16_4C_BOARD_DEVICE_ID:
		case xpcie3121_8_8C_BOARD_DEVICE_ID:
		case xpcie3121_8_4C_BOARD_DEVICE_ID:
		case xpcie3521_8_BOARD_DEVICE_ID:
		case xpcie3521_4_BOARD_DEVICE_ID:
		case xpcie3521_8C_BOARD_DEVICE_ID:
		case xpcie3521_4C_BOARD_DEVICE_ID:
		{
			if (((XPCI3XXX_PRIVDATA(pdev)->b_AnalogOutputInitialisedVoltageMode == ADDIDATA_UNIPOLAR) && (dw_WriteValue > 32767)) ||
				((XPCI3XXX_PRIVDATA(pdev)->b_AnalogOutputInitialisedVoltageMode == ADDIDATA_BIPOLAR) && (dw_WriteValue > 65535)))
				return 4;
		}
		break;

		default:
		{
			if (((XPCI3XXX_PRIVDATA(pdev)->b_AnalogOutputInitialisedVoltageMode == ADDIDATA_UNIPOLAR) && (dw_WriteValue > 2047)) ||
				((XPCI3XXX_PRIVDATA(pdev)->b_AnalogOutputInitialisedVoltageMode == ADDIDATA_BIPOLAR) && (dw_WriteValue > 4095)))
				return 4;
		}
		break;
	}

	/* Write the digital value of the output */
	/* Value is B0?B31 dw_WriteValueDWord ? dw_WriteValueDWord & 0xFFFFFFFF */

	/* Evaluate the channel number, DAC Value and polarity */
	if (XPCI3XXX_PRIVDATA(pdev)->b_AnalogOutputInitialisedVoltageMode == ADDIDATA_UNIPOLAR)
		{
		dw_CommandRegisterPolarity = 0x80000000L;
		}
	else
		{
		if (XPCI3XXX_PRIVDATA(pdev)->b_AnalogOutputInitialisedVoltageMode == ADDIDATA_BIPOLAR)
			{
			dw_CommandRegisterPolarity = 0x0;
			}
		}

	dw_CommandRegister = (uint32_t) ((uint32_t)(b_Channel & 0xFF) | (uint32_t) ((dw_WriteValue << 0x8) & 0x7FFFFF00L) | (uint32_t) dw_CommandRegisterPolarity);

	/* Set the channel number, DAC Value and polarity */
	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		OUTPDW_MEM(GET_MEM_BAR3(pdev), (XPCI3XXX_PRIVDATA(pdev)->dw_AnalogOutputOffsetAdress + 4), dw_CommandRegister);
	else
		OUTPDW((GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_AnalogOutputOffsetAdress + 4), dw_CommandRegister);

/*	OUTP_WAIT_READY   (b_CallLevel,
	                     ps_HardwareInformation,
	                     4,
	                     dw_CommandRegister3,
	                     0,
	                     NULL,
	                     0,
	                     0);*/


	return 0;
}

///** Reads the EOC Bit of the analog output conversion.
// *
// * This function doesn't check parameters validity.
// * It is written to have a short execution time.
// * Use it only if you are sure of what you are doing.
// *
// * @param[in] pdev									: PCI Device struct.
// * @param[out] pb_AnalogOutputConversionFinished	: 0: No analog output conversion finished
// *													  1: Analog output conversion finished
// *
// * @return   0 : No error                                           \n
// *                       1 : The handle parameter of the board is wrong         \n
// */
//int i_xpci3xxx_FastReadAnalogOutputBit (struct pci_dev * pdev)
//{
//	uint32_t dw_CommandRegister = 0;
//
//	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
//		INPDW_MEM(GET_MEM_BAR3(pdev), XPCI3XXX_PRIVDATA(pdev)->dw_AnalogOutputOffsetAdress, &dw_CommandRegister);
//	else
//		INPDW((GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_AnalogOutputOffsetAdress), &dw_CommandRegister);
//
//	if ( (dw_CommandRegister & 0x100) == 0x100 )
//		return 1;
//
//	return 0;
//}

/** Writes the value of the analog output channel.
 *
 * This function doesn't check parameters validity.
 * It is written to have a short execution time.
 * Use it only if you are sure of what you are doing.
 *
 * @param[in] pdev			: PCI Device struct.
 * @param[in] b_Channel		: channel number of the output to be initialise.
 * @param[in] dw_WriteValue	: Value to write on the analog output channel.

 * @retval 0 Always 0.
 */
int i_xpci3xxx_FastWriteAnalogOutputValue (struct pci_dev * pdev, uint8_t b_Channel, uint32_t dw_WriteValue)
{
	uint32_t dw_CommandRegister = 0;
	uint32_t dw_CommandRegisterPolarity = 0;

	if (XPCI3XXX_PRIVDATA(pdev)->b_AnalogOutputInitialisedVoltageMode == ADDIDATA_UNIPOLAR)
	{
		dw_CommandRegisterPolarity = 0x80000000L;
	}

	dw_CommandRegister = (uint32_t) ((uint32_t)(b_Channel & 0xFF) | (uint32_t) ((dw_WriteValue << 0x8) & 0x7FFFFF00L) | (uint32_t) dw_CommandRegisterPolarity);

	/* Set the channel number, DAC Value and polarity */
	if (pdev->device != xpci3501_BOARD_DEVICE_ID)
		OUTPDW_MEM(GET_MEM_BAR3(pdev), (XPCI3XXX_PRIVDATA(pdev)->dw_AnalogOutputOffsetAdress + 4), dw_CommandRegister);
	else
		OUTPDW((GET_BAR1(pdev) + XPCI3XXX_PRIVDATA(pdev)->dw_AnalogOutputOffsetAdress + 4), dw_CommandRegister);

	return 0;
}

