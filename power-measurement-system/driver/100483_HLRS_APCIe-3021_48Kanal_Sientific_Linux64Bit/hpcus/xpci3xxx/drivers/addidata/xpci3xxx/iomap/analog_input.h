/* @file iomap.h
 *
 * @brief Definition of the iomapping of the board
 *
 * This file contains defines, enums, etc... that describes the register layout of the board
 *
 */

/** @par LICENCE
 *
 *  @verbatim
  Copyright (C) 2004,2005 ADDI-DATA GmbH for the source code of this module.

         ADDI-DATA GmbH
         Dieselstrasse 3
         D-77833 Ottersweier
         Tel: +19(0)7223/9493-0
         Fax: +49(0)7223/9493-92
         http://www.addi-data-com
         info@addi-data.com

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the
 Free Software Foundation; either version 2.1 of the License,
 or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this library; if not, write to the Free Software Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA


 You also shoud find the complete LGPL in the LGPL.txt file accompanying
 this source code.

 @endverbatim
**/

#ifndef __APCI3XXX_IOMAP_ANALOG_INPUT_H__
#define __APCI3XXX_IOMAP_ANALOG_INPUT_H__

/* analog-input */
/* BAR 3 */

/*
Offset + 0: (WRITE)
	SEQ = 0 Write the channel configuration
		GAIN:          Define the gain of this channel
					  000000 : First Gain available
							   000001 : Second Gain available
			  U/B#:            Define the polarity of this channel
							   0 : Bipolar
							   1 : Unipolar
		D/S#: Define the input mode: Single-ended or differential.
					  0 : Single-ended
							   1 : Differential
	SEQ = 1 Write the sequence / Scan index
		   Channel index: Determine the channel to the index
*/
#define REG_AI_CHANNEL_CONFIG (0)

static inline uint32_t read_reg_ai_channel_config(struct pci_dev * pdev)
{
	return readl(GET_MEM_BAR3(pdev) + REG_AI_CHANNEL_CONFIG);
}

static inline void write_reg_ai_channel_config(struct pci_dev * pdev, uint32_t mask, uint32_t val)
{
	uint32_t newval = ( read_reg_ai_channel_config(pdev) & (~mask) ) | val;
	writel( newval, GET_MEM_BAR3(pdev) + REG_AI_CHANNEL_CONFIG);
}

/*
Offset + 4: (WRITE)
	Index: Define the index of the sequence or channel configuration array .
	SEQ:           0: Channel configuration selected
				   1: Sequence configuration selected
	Delay mode:    Define the mode of the delay :
				   000: Delay is used between 2 cycle of acquisition .By start of
						 the acquisition (software or trigger ext), the acquisition
						is started and the delay is actived after this acquisition
						cycle.
				   001: Delay is used between 2 cycle of acquisition but by start of
						 the acquisition (software or trigger ext), the acquisition is
						do after the delay
	0V Cali:       0: 0V Calibration deactivate
				   1: 0V Calibration activate
	5V Cali:       0: 5V Calibration deactivate
				   1: 5V Calibration activate
	-5V Cali:      0: -5V Calibration deactivate
				   1: -5V Calibration activate
	U/B Cali:      Define the calibration polarity
				   0 : Bipolar
				   1 : Unipolar
*/
#define REG_AI_SEQUENCE_CONTROL (4)

static inline uint32_t read_reg_ai_sequence_control(struct pci_dev * pdev)
{
	return readl(GET_MEM_BAR3(pdev) + REG_AI_SEQUENCE_CONTROL);
}

static inline void write_reg_ai_sequence_control(struct pci_dev * pdev, uint32_t mask, uint32_t val)
{
	uint32_t newval = (read_reg_ai_sequence_control(pdev) & (~mask)) | val ;
	writel( newval, GET_MEM_BAR3(pdev) + REG_AI_SEQUENCE_CONTROL);
}

/*
Offset + 8: (WRITE)
	Start sequence/SCAN index:Set the start index for the sequence array / SCAN
							   conversion
	Stop sequence/SCAN index:Set the stop index for the sequence array / SCAN
							   conversion
	Seq / SCAN Mode:           Define the mode of the acquisition :
							   00 Do the sequence / SCAN cycle x
									time (x is the value of the counter).
								   (Counter on Adress + 48 has to be initialised)
							   01: Do always the sequence / SCAN
									cycle (continuous)
	Delay:                     0: Disable the delay
							   1: Enable the delay
	Start:                     Allow to start the acquisition
							   0: Stop the acquisition
							   1: Start the acquisition.
								 It’s set automatically to 0 after 1
								 cycle stopped and Seq/SCAN Mode = 00 or
								 after the last cycle stopped and Seq/SCAN
								 Mode = 01
	IRQ:                       0: Disable the interrupt
							   1: Enable the interrupt
 */
#define REG_AI_SEQUENCE_CONFIG (8)

static inline uint32_t read_reg_ai_sequence_config(struct pci_dev * pdev)
{
	return readl(GET_MEM_BAR3(pdev) + REG_AI_SEQUENCE_CONFIG);
}

static inline void write_reg_ai_sequence_config(struct pci_dev * pdev, uint32_t mask, uint32_t val)
{
	uint32_t newval = ( read_reg_ai_sequence_config(pdev) & (~mask) ) | val;
	writel( newval, GET_MEM_BAR3(pdev) + REG_AI_SEQUENCE_CONFIG);
}

static inline int reg_ai_is_acquisition_started(struct pci_dev * pdev)
{
	return ( read_reg_ai_sequence_config(pdev) & 0x80000 );
}

/*
Offset + 12: (WRITE)
	Clear Index:       Allow to initialise the Index of the sequence.
					   When the acquisition is stopped and restarted
					   Clear Index is on 0, the acquisition carry on the
					   sequence.
					   Clear Index is on 1, the index of the sequence is
					   initialised and the acquisition begin on the first element
					   of the sequence.
*/
#define REG_AI_CLEAR_INDEX (12)

static inline uint32_t read_reg_ai_clear_index(struct pci_dev * pdev)
{
	return readl(GET_MEM_BAR3(pdev) + REG_AI_CLEAR_INDEX);
}

static inline void write_reg_ai_clear_index(struct pci_dev * pdev, uint32_t mask, uint32_t val)
{
	uint32_t newval = (read_reg_ai_clear_index(pdev) & (~mask)) | val ;
	writel( newval, GET_MEM_BAR3(pdev) + REG_AI_CLEAR_INDEX);
}

/*
Offset + 16: (READ)
	EOS IRQ 0: No end of sequence/SCAN interrupt occur
			1: End of sequence/SCAN interrupt occur.
			   It’s reset after a dummy “1” write on this
			   address on this bit.
 */
#define REG_AI_INTERRUPT_STATUS (16)

static inline uint32_t read_reg_ai_interrupt_status(struct pci_dev * pdev)
{
	return readl(GET_MEM_BAR3(pdev) + REG_AI_INTERRUPT_STATUS);
}

static inline void write_reg_ai_interrupt_status(struct pci_dev * pdev, uint32_t val)
{
	writel( val, GET_MEM_BAR3(pdev) + REG_AI_INTERRUPT_STATUS);
}
/*
Address + 20 :
	EOS:					0 : End of sequence/SCAN not occur
							1 : End of sequence/SCAN occur.
								Reset after reading the last value from sequence or SCAN
	Burn Out Status : 		0: input connection Error
							1: Input connection OK
	Short-circuit Status :	0 : no Short –circuit on the input
							1 : Short-circuit detected on the input

 */
#define REG_AI_ACQUISITION_STATUS (20)
/*
Address + 28 :
	Channel value:		Channel value
*/
#define REG_AI_CHANNEL_VALUE (28)

/*
Offset + 32: (WRITE)
	Reload conversion time value:            Conversion time value.
*/
#define REG_AI_CONVERSION_TIME_VALUE (32)

static inline void write_reg_ai_conversion_time_value(struct pci_dev * pdev, uint32_t val)
{
	writel( val, GET_MEM_BAR3(pdev) + REG_AI_CONVERSION_TIME_VALUE);
}

/*
Offset + 36: (WRITE)
	Conversion time - time base: Define the time base of the conversion
                                                      time
                                                      01 : µs
                                                      10 : ms
                                                      11 : s
*/
#define REG_AI_CONVERSION_TIME_UNIT (36)

static inline void write_reg_ai_conversion_time_unit(struct pci_dev * pdev, uint32_t val)
{
	writel( val, GET_MEM_BAR3(pdev) + REG_AI_CONVERSION_TIME_UNIT);
}

/*
Offset + 40: (WRITE)
	Reload the delay time value: Value of the delay timer.
*/
#define REG_AI_DELAY_TIMER_VALUE (40)

static inline void write_reg_ai_delay_timer_value(struct pci_dev * pdev, uint32_t val)
{
	writel( val, GET_MEM_BAR3(pdev) + REG_AI_DELAY_TIMER_VALUE);
}

/*
Offset + 44: (WRITE)
	Delay time base:                     Define the time base of the delay timer
												01 : µs
												10 : ms
												11 : s
*/
#define REG_AI_DELAY_TIMER_UNIT (44)

static inline void write_reg_ai_delay_timer_unit(struct pci_dev * pdev, uint32_t val)
{
	writel( val, GET_MEM_BAR3(pdev) + REG_AI_DELAY_TIMER_UNIT);
}

/*
Offset + 48: (WRITE)
	Number of seq / SCAN (counter):      Define the number of SCAN/SEQ to
												acquire.
*/
#define REG_AI_SEQUENCE_COUNT (48)

static inline void write_reg_ai_sequence_count(struct pci_dev * pdev, uint32_t val)
{
	writel( val, GET_MEM_BAR3(pdev) + REG_AI_SEQUENCE_COUNT);
}

/*
Offset + 52: (WRITE)
	Number of seq trigger:               Define the number of sequence to acquire
												after the trigger occur (Sequence trigger
												mode) to acquire
*/
#define REG_AI_SEQUENCE_TRIGGER_COUNT (52)

/*

Address + 56:
	Number of DMA to transfer : Define the number of DMA Transfer
			   - Not equal to 0	= Simple mode. The firmware stop the acquisition automatically after all DMA Transfer
					    occur
			   - Equal to 0 	= Continuous mode. The Software stop the acquisition.
 */
#define REG_AI_NUMBER_DMA_TRANSFER (56)

static inline void write_reg_ai_number_dma_transfer(struct pci_dev * pdev, uint32_t val)
{
	writel( val, GET_MEM_BAR3(pdev) + REG_AI_NUMBER_DMA_TRANSFER);
}

/*
Offset + 64: (WRITE)
	Hardware trigger configuration:     Configure the hardware trigger.
										Bit 0: 1 : enable the trigger
												  0 : disable the trigger
										Bit 1,2 : define the active front (Only if
												   hardware trigger selected)
												  01: ↑ front
												  10: ↓ front
												  11: Both front
										Bit 3,4,5,6 : Define the trigger mode
														   0000: One shot trigger
														   0001: Single channel
																 trigger
														   0010: Sequence trigger
																 Address + 52 define
																 the number of
																 sequence to acquire
														  0011: Single sequence
																 trigger
			 Ext Trigger count value: Define the number of trigger to wait that the firmware take in account the trigger.
*/
#define REG_AI_HW_TRIGGER_CONFIG (64)

static inline uint32_t read_reg_ai_hw_trigger_config(struct pci_dev * pdev)
{
	return readl(GET_MEM_BAR3(pdev) + REG_AI_HW_TRIGGER_CONFIG);
}

static inline void write_reg_ai_hw_trigger_config(struct pci_dev * pdev, uint32_t mask, uint32_t val)
{
	uint32_t newval = ( read_reg_ai_hw_trigger_config(pdev) & (~mask) ) | val;
	writel( newval, GET_MEM_BAR3(pdev) + REG_AI_HW_TRIGGER_CONFIG);
}

/*

Offset + 68: (READ)
	Trigger count value: Return the number of triggers to wait before the firmware takes into account the trigger.
	Hard Trg Status     0: No hardware trigger occur
						1: Hardware trigger occur.
							If is set to 1 after Trigger count value
							equal Ext Trigger count value
	Hard Trg State 		0: Hardware trigger input is low
						1: Hardware trigger input is high
 */
#define REG_AI_HW_TRIGGER_STATUS (68)

/*
Offset + 72: (WRITE)
Software trigger configuration: Configure the software trigger.
		   Bit 0: 1: enable the trigger
					0: disable the trigger
		   Bit 1,2,3,4 : Define the trigger mode
						 0000: One shot trigger
						 0001: Single channel trigger
						 0010: Sequence trigger
							   Address + 52 define
							   the number of
							   sequence to acquire
						 0011: Single sequence trigger
Soft Trg    0: No action
			1: Software trigger
*/
#define REG_AI_SW_TRIGGER_CONFIG (72)

static inline uint32_t read_reg_ai_sw_trigger_config(struct pci_dev * pdev)
{
	return readl(GET_MEM_BAR3(pdev) + REG_AI_SW_TRIGGER_CONFIG);
}

static inline void write_reg_ai_sw_trigger_config(struct pci_dev * pdev, uint32_t mask, uint32_t val)
{
	uint32_t newval = ( read_reg_ai_sw_trigger_config(pdev) & (~mask) ) | val;
	writel( newval, GET_MEM_BAR3(pdev) + REG_AI_SW_TRIGGER_CONFIG);
}

/*
Offset + 76: (READ)
             Soft Trg Status 0: No software trigger occur
                             1: Software trigger occur

*/
#define REG_AI_SW_TRIGGER_STATUS (76)

/*
Address + 80
GATE :		Configure the gate input mode (this is a digital input)
			Bit 0 :	1 : enable the input gate.
				0 : disable the input gate.
			Bit 1 :	0 : enable the acquisition on the low level
 				1 : enable the acquisition on the high level
 */

#define REG_AI_GATE_INPUT_CONFIG (80)

static inline void write_reg_ai_gate_input_config(struct pci_dev * pdev, uint32_t val)
{
	writel( val, GET_MEM_BAR3(pdev) + REG_AI_GATE_INPUT_CONFIG);
}

/* PCI bus accesses are asynchronous.
 * It may be necessary to perform a read to flush previous writes.
 * I have chosen an empty register (NS).
 */

static inline void flush_reg_ai(struct pci_dev * pdev)
{
	readl( GET_MEM_BAR3(pdev) + 32);
}

#endif // __APCI3XXX_IOMAP_ANALOG_INPUT_H__
