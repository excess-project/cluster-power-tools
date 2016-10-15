/** @file xpci3xxx.h
 *
 * @brief Define the public API for the xpci3xxx ADDI-DATA board.
 *
 *
 */
/** \mainpage xPCI3xxx IOCTL API
* $MainRevision: $
* \section intro_sec Introduction
*
* The xpci3xxx Linux driver supports the following boards:
* - apci3003
* - apci3002
* - apci3116
* - apci3110
* - apci3106
* - apci3100
* - apci3016
* - apci3110
* - apci3106
* - apci3100
* - apci3016
* - apci3010
* - apci3006
* - apci3000
* - apci3500
* - apci3002
* - apci3501
*
* This document describes the IOCTL interface provided by the xPCI3xxx driver for normal applications.
*
* \subsection ioctl IOCTL commands
*
* ioctl() is a function which takes three arguments:
* - a file descriptor, which corresponds to a device
* - a command, specific to the device
* - a pointer to memory, which contains the arguments for the command. This argument differs from one command to the other.
*
* Usually, on success zero is returned, on  error,  -1  is returned, and errno is set appropriately.
* Commands may also return a positive error value, whose signification is to be found in the command documentation.
*
* A typical use could be:
* @verbatim
inf fd = open( ... );
if ( -1 == ioctl(fd, CMD_...,  ) )
	perror("ioctl");
close(fd);
@endverbatim
*
* \subsection more Where to find more information?
*
* The chapter "Modules" contains a detailed description of the commands, with parameter and return value.\n
*
* The driver comes also with a set of samples, which exemplifies the use of the IOCTL commands.
*
* For each C function, such as ioctl(), there is documentation available in the form of "man pages".
* Try in the console
* @verbatim
$ man ioctl
@endverbatim
* to know if the relevant documentation package is already installed.
*
* The installation procedure differs form one distribution to another.
* For instance, for Debian/Ubuntu, it is
* @verbatim
$ apt-get install manpages-dev
@endverbatim
*
*/

/** @par LICENCE
*
*	@verbatim
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

#ifndef __xpci3xxx_H__
#define __xpci3xxx_H__


#include <apci.h>

//GENERAL DEFINE

/** \internal */
#define xpci3xxx_BOARD_NAME					"xpci3xxx"

/** @internal */
#define xpci3xxx_BOARD_VENDOR_ID				0x15B8

/** @internal */
#define xpci3002_16_BOARD_DEVICE_ID			0x3002

/** @internal */
#define xpci3002_8_BOARD_DEVICE_ID				0x3003

/** @internal */
#define xpci3002_4_BOARD_DEVICE_ID				0x3004

/** @internal */
#define xpci3003_BOARD_DEVICE_ID				0x300b

/** @internal */
#define xpci3000_16_BOARD_DEVICE_ID			0x3010

/** @internal */
#define xpci3000_8_BOARD_DEVICE_ID				0x3011

/** @internal */
#define xpci3000_4_BOARD_DEVICE_ID				0x3012

/** @internal */
#define xpci3006_16_BOARD_DEVICE_ID			0x3013

/** @internal */
#define xpci3006_8_BOARD_DEVICE_ID				0x3014

/** @internal */
#define xpci3006_4_BOARD_DEVICE_ID				0x3015

/** @internal */
#define xpci3010_16_BOARD_DEVICE_ID			0x3016

/** @internal */
#define xpci3010_8_BOARD_DEVICE_ID				0x3017

/** @internal */
#define xpci3010_4_BOARD_DEVICE_ID				0x3018

/** @internal */
#define xpci3016_16_BOARD_DEVICE_ID			0x3019

/** @internal */
#define xpci3016_8_BOARD_DEVICE_ID				0x301a

/** @internal */
#define xpci3016_4_BOARD_DEVICE_ID				0x301b

/** @internal */
#define xpci3100_16_BOARD_DEVICE_ID			0x301c

/** @internal */
#define xpci3100_8_BOARD_DEVICE_ID				0x301d

/** @internal */
#define xpci3106_16_BOARD_DEVICE_ID			0x301e
/** @internal */
#define xpci3106_8_BOARD_DEVICE_ID				0x301f

/** @internal */
#define xpci3110_16_BOARD_DEVICE_ID			0x3020
/** @internal */
#define xpci3110_8_BOARD_DEVICE_ID				0x3021

/** @internal */
#define xpci3116_16_BOARD_DEVICE_ID			0x3022
/** @internal */
#define xpci3116_8_BOARD_DEVICE_ID				0x3023

/** @internal */
#define xpci3500_BOARD_DEVICE_ID					0x3024

/** @internal */
#define xpci3501_BOARD_DEVICE_ID					0x3001

/** @internal */
#define xpci3002_16_AUDI_BOARD_DEVICE_ID		0x3029

/** @internal */
#define xpci3009_BOARD_DEVICE_ID		0x3028

/** @internal */
#define xpcie3121_16_8_BOARD_DEVICE_ID		0x302E
#define xpcie3121_16_4_BOARD_DEVICE_ID		0x302F
#define xpcie3121_8_8_BOARD_DEVICE_ID		0x3030
#define xpcie3121_8_4_BOARD_DEVICE_ID		0x3031
#define xpcie3121_16_8C_BOARD_DEVICE_ID		0x3032
#define xpcie3121_16_4C_BOARD_DEVICE_ID		0x3033
#define xpcie3121_8_8C_BOARD_DEVICE_ID		0x3034
#define xpcie3121_8_4C_BOARD_DEVICE_ID		0x3035

#define xpcie3021_16_BOARD_DEVICE_ID		0x3036
#define xpcie3021_8_BOARD_DEVICE_ID			0x3037
#define xpcie3021_4_BOARD_DEVICE_ID			0x3038

#define xpcie3521_8_BOARD_DEVICE_ID			0x3039
#define xpcie3521_4_BOARD_DEVICE_ID			0x303A
#define xpcie3521_8C_BOARD_DEVICE_ID		0x303B
#define xpcie3521_4C_BOARD_DEVICE_ID		0x303C



/* value used for CMD_xpci3xxx_CheckAndGetPCISlotNumber ! DEPRECATED ! */
/** @deprecated */
#ifndef CONFIG_xpci3xxx_MAX_BOARD_NBR
#define CONFIG_xpci3xxx_MAX_BOARD_NBR					(3)
#endif // CONFIG_xpci3xxx_MAX_BOARD_NBR

/** @deprecated */
#define xpci3xxx_MAX_BOARD_NBR CONFIG_xpci3xxx_MAX_BOARD_NBR

/** @internal */
#define xpci3xxx_MAGIC (char) 'A'

/** @deprecated */
struct xpci3xxx_USER_str_BoardInformations
{
	// FILLME
	uint8_t	b_SlotNumber;			/**< PCI slot number */
	uint8_t	b_InterruptNbr;		/**< Board interrupt number */
	uint32_t dw_BoardBaseAddress [5]; /**< array of BAR base address (in kernel virtual space) */
};

#define ADDIDATA_DIFFERENTIAL	1
#define ADDIDATA_SINGLE			0

#define ADDIDATA_ENABLE			1
#define ADDIDATA_DISABLE		0

#define	ADDIDATA_DIGITAL_INPUT	0
#define	ADDIDATA_DIGITAL_OUTPUT	1

#define	ADDIDATA_TIMER			4
#define	ADDIDATA_WATCHDOG		5
#define	ADDIDATA_COUNTER		7

#define ADDIDATA_ENABLE			1
#define ADDIDATA_DISABLE		0

#define ADDIDATA_UP				1
#define ADDIDATA_DOWN 			0

#define ADDIDATA_LOW			1
#define ADDIDATA_HIGH 			2
#define ADDIDATA_LOW_HIGH		3

#define ADDIDATA_OR				1
#define ADDIDATA_AND			2

#define ADDIDATA_MAX_TCW		3

#define ADDIDATA_MAX_AI			16
#define ADDIDATA_1_GAIN			0x0
#define ADDIDATA_2_GAIN			0x1
#define ADDIDATA_5_GAIN			0x2
#define ADDIDATA_10_GAIN		0x3

#define ADDIDATA_BIPOLAR		0x0
#define ADDIDATA_UNIPOLAR		0x1

#define ADDIDATA_NANO_SECOND	0x0
#define ADDIDATA_MICRO_SECOND	0x1
#define ADDIDATA_MILLI_SECOND	0x2
#define ADDIDATA_SECOND			0x3

#define ADDIDATA_DELAY_MODE_1	1
#define ADDIDATA_DELAY_MODE_2	2
#define ADDIDATA_EOC			0x1U

/**
 * \defgroup general General functions
 */
/**
 * \defgroup analogin Analog Inputs
 */

/**
 * \defgroup autorefresh Autorefresh mode
 */

/**
 * \defgroup sequence Sequence mode
 */

/**
 * \defgroup analogout Analog Outputs
 */

/**
 * \defgroup digin Digital Inputs
 */

/**
 * \defgroup digout Digital outputs
 */

/**
 * \defgroup counter Counter
 */

/**
 * \defgroup watchdog Watchdog
 */

/**
 * \defgroup timer Timer
 */

/** \addtogroup general
 * @{
 */

/** Returns the informations of base address, IRQ to the user.
*
* @param[out] (struct xpci3xxx_USER_str_BoardInformations)
*
* @deprecated
*/
#define CMD_xpci3xxx_GetHardwareInformation _IOR(xpci3xxx_MAGIC, 1,long)

/** Retrieve information about the older event in the event FIFO.
*
* Events such as interrupts are stored in a FIFO of size ADDIDATA_MAX_EVENT_COUNTER.
* This function retrieve the older information present in the FIFO and frees the corresponding slot.
*
* @param[out] uint32_t[1 + ADDIDATA_MAX_AI + 1 + ADDIDATA_MAX_INTERRUPT_ANALOG_INPUT_VALUE]
*
* index 0 of the array contains the source of the interrupt. It may take the following values:
@verbatim
0xfffffffe	FIFO is Empty
0xffffffff 	FIFO is Full
0x1 		Analog input with DMA
0x2 		Analog input without DMA
0x4 		Analog input FIFO Overflow
0x10		TimerCounterWatchdog 0
0x20		TimerCounterWatchdog 1
0x40		TimerCounterWatchdog 2
0x100		TimerCounterWatchdog 0
0x200 		TimerCounterWatchdog 1
0x400 		TimerCounterWatchdog 2
0x1000		TimerCounterWatchdog 0
0x2000		TimerCounterWatchdog 1
0x4000		TimerCounterWatchdog 2
@endverbatim
*
* If the interrupt has been generated by analog input, without DMA (0x2),
* then the array index from 1 to (ADDIDATA_MAX_AI+1) is filled with the values acquired.
* Only the indexes corresponding to actual inputs are significant.
*
* If the interrupt has been generated by analog input using DMA (0x1),
* then the array index from (ADDIDATA_MAX_AI+2) to (ADDIDATA_MAX_AI+2+ADDIDATA_MAX_INTERRUPT_ANALOG_INPUT_VALUE)
* is filled with the values acquired.
* Only the indexes corresponding to actual inputs are significant.
*
* @retval 0 success
* @retval -1 an error happened and errno is set appropriately.
*/
#define CMD_xpci3xxx_TestInterrupt _IOR(xpci3xxx_MAGIC, 2,long)

/** Configures the bidirectional TTL I/O.
*
* TTL I/O are grouped in three ports:
* - port 1 =  8 TTL inputs
* - port 2 = 8 TTL outputs
* - port 3 = 8 TTL bi-directional, which can be either input or output
*
* This command allows to configure each TTL I/O of port 3 in either input or output.
*
* @param[in] uint8_t*arg = 0 input (default) | 1 : output
*
* @retval 0 : Success
* @retval 1 : Internal error
* @retval 2 : TTL functionality not supported
*
* @note the content of the register that contains the state of these TTLs
* is not synchronized by this command. After changing this setting from output as input
* the command CMD_xpci3xxx_Read32DigitalInputs must be called twice.
*
*/
#define CMD_xpci3xxx_SetTTLPortConfiguration _IOR(xpci3xxx_MAGIC, 3,long)

/** @} */

/** \addtogroup analogin
* @{ */

/** Initializes one analog input channel.
 *
 * @param[in] (uint8_t *)arg[4] Parameters
 *
 *
 *	- arg[0] channel number of the input to be initialized
 *	- arg[1] Hardware configuration = 0 : Single ended | 1 : Differential
 *	- arg[2] Gain factor of the analog input
 *						- ADDIDATA_1_GAIN : Gain =  1
 *						- ADDIDATA_2_GAIN : Gain =  2
 *						- ADDIDATA_5_GAIN : Gain =  5
 *						- ADDIDATA_10_GAIN: Gain = 10
 *	- arg[4] Polarity of the analog input
 *						- ADDIDATA_UNIPOLAR:  0V - +10V
 *						- ADDIDATA_BIPOLAR: -10V - +10V
 *
 * @retval 0 Success
 * @retval 1 Internal error
 * @retval 2 Wrong channel number
 * @retval 3 Wrong single/diff value
 * @retval 4 Wrong gain
 * @retval 5 Wrong polarity
 * @retval 6 A conversion is already started
 * @retval 7 Requested single/diff parameter not supported by the board
 *
 */
#define CMD_xpci3xxx_InitAnalogInput _IOR(xpci3xxx_MAGIC, 10,long)


/** Starts the conversion of one analog input channel.
 *
 * @param[in] uint16_t arg[3]
 *
 *	- arg[0]	Channel number
 *	- arg[1]	b_ConvertTimeUnit : Convert time unit for the analog input:
 *                                              - ADDIDATA_NANO_SECOND : nanoseconds
 *                                              - ADDIDATA_MICRO_SECOND : microseconds
 *                                              - ADDIDATA_MILLI_SECOND : milliseconds
 *	- arg[2]	b_ConvertTimeUnit : Convert time value for the analog input conversion
 *
 * @retval 0 Success
 * @retval 1 Internal error
 * @retval 2 Wrong channel number
 * @retval 3 Channel not initialized
 * @retval 4 Wrong convert time unit
 * @retval 5 Wrong convert time
 * @retval 6 A conversion is already started
 */

#define CMD_xpci3xxx_StartAnalogInput _IOR(xpci3xxx_MAGIC, 11,long)

/** Reads the End Of Conversion Bit of the analog input conversion.
 *
 * @param[out] (uint8_t *arg) Status of the EOC bit = 0: no analog input conversion finished | 1: analog input conversion finished
 *
 * @retval 0 Success
 * @retval 1 Internal error
 */

#define CMD_xpci3xxx_ReadAnalogInputBit _IOR(xpci3xxx_MAGIC, 12,long)

/** Reads the value of the analog input channel.
 *
 * @param[out] uint32_t *arg = Value of the analog input channel
 *
 * @retval 0 Success
 * @retval 1 Internal error
 */
#define CMD_xpci3xxx_ReadAnalogInputValue _IOR(xpci3xxx_MAGIC, 13,long)

/** @} */

/* AUTOREFRESH */
/** \addtogroup autorefresh
* @{ */

/** Initialise an analog input auto-refresh acquisition.
 *
 * This function prepare the board for an acquisition in auto-refresh mode
 * and allocate the relevant resources. A call to CMD_xpci3xxx_ReleaseAnalogInputAutoRefresh
 * is necessary to release theses resources before being able to start another type of acquisition.
 *
 * @param[in]  uint32_t *arg[(3 * ADDIDATA_MAX_AI) + 6] contains initialization parameters - Must be a pointer to an array of maximum 54 uint32_t (216 bytes) -
 *
 * The ordered list of parameters is given below.
 *
 *	- arg[0] : Number of channel to initialize (1 .. ADDIDATA_MAX_AI)
 *	- arg[1] .. arg[arg[0]-1] : List of channels to initialize.
 *	- arg[arg[0]] .. arg[2*arg[0]-1] : Gain factor array of the channels
 *                                          - ADDIDATA_1_GAIN : Gain =  1
 *                                          - ADDIDATA_2_GAIN : Gain =  2
 *                                          - ADDIDATA_5_GAIN : Gain =  5
 *                                          - ADDIDATA_10_GAIN: Gain = 10
 *	- arg[2*arg[0]] .. arg[3*arg[0]-1] : Polarity of the channels
 *                                          - ADDIDATA_UNIPOLAR:  0V - +10V
 *                                          - ADDIDATA_BIPOLAR: -10V - +10V
 *	- arg[3*arg[0]+1] : Hardware configuration = 0 : Single ended HW config  | 1 : Differential HW config
 *	- arg[3*arg[0]+2] : Number of sequence = 0 : continuous mode | 1: single mode
 *	- arg[3*arg[0]+3] : Delay mode for the analog input
 *                                              - ADDIDATA_DELAY_MODE_1: Mode 1
 *                                              - ADDIDATA_DELAY_MODE_2: Mode 2
 *	- arg[3*arg[0]+4] : Delay time unit for the analog input:
 *                                              - ADDIDATA_MICRO_SECOND :micros
 *                                              - ADDIDATA_MILLI_SECOND :ms
 *                                              - ADDIDATA_SECOND :s
 *	- arg[3*arg[0]+5] : Delay time value for the analog input conversion
 *
 * @retval 0  Success
 * @retval 1  Internal error
 * @retval 2  Wrong number of channels
 * @retval 3  Wrong channel number
 * @retval 4  Wrong gain factor
 * @retval 5  Wrong polarity
 * @retval 6  Wrong single diff parameter (not 0 or 1)
 * @retval 7  Wrong delay mode parameter
 * @retval 8  Wrong delay time unit parameter
 * @retval 9  Wrong delay time parameter
 * @retval 10 A conversion is already started
 * @retval 11 requested single diff parameter not supported by the board
 *
 * @note
 *
 * <b>More on pb_ChannelList</b>
 *
 * b_NumberOfChannels describes the sequence to follow during acquisition. It must contain indexes of valid channels.
 * Which index is valid depends on the connection mode of the board (single or differential).
 *
 * The order of the index in this array have no influence on the order of the values
 * returned after an acquisition.
 *
 * <b>Constraints on parameter NumberOfChannel</b>
 * let NumberOfAnalogInput be the number of analog inputs (4,8,16) of the board, then NumberOfChannel is correct if
 * - 0 < NumberOfChannel <= ADDIDATA_MAX_AI
 * - if the board is in differential mode, NumberOfChannel < (b_NumberOfAnalogInput/2)
 * - if the board is in single mode, NumberOfChannel < b_NumberOfAnalogInput
 *
 *
 * <b>Constraints on parameter pb_ChannelList</b>
 * let NumberOfAnalogInput be the number of analog inputs (4,8,16) of the board, then NumberOfChannel is correct if:
 * - if the board is in differencial mode, for each value in pb_ChannelList, value < (b_NumberOfAnalogInput/2)
 *
 */
#define CMD_xpci3xxx_InitAnalogInputAutoRefresh _IOR(xpci3xxx_MAGIC, 14,long)


/** Start an analog input auto refresh conversion.
 *
 * @param[in] uint16_t arg[2]
 *
 *	- arg[0] Convert time unit for the analog input:
 *                                              - ADDIDATA_NANO_SECOND : nanoseconds
 *                                              - ADDIDATA_MICRO_SECOND : microseconds
 *                                              - ADDIDATA_MILLI_SECOND : milliseconds
 *	- arg[1] Convert time value for the analog input conversion
 *
 * @retval 0 Success
 * @retval 1 Internal error
 * @retval 2 Auto refresh not initialized
 * @retval 3 Wrong convert time unit
 * @retval 4 Wrong convert time
 * @retval 5 A conversion is already started
 */
#define CMD_xpci3xxx_StartAnalogInputAutoRefresh _IOR(xpci3xxx_MAGIC, 15,long)

/** Stop an analog input auto refresh conversion
 *
 * @param NULL (ignored)
 *
 * @retval 0 Success
 * @retval 1 Internal error
 * @retval 2 Auto refresh not initialized
 */
#define CMD_xpci3xxx_StopAnalogInputAutoRefresh _IOR(xpci3xxx_MAGIC, 16,long)

/** Read the auto refresh values
 *
 * @param[out] uint32_t[ADDIDATA_MAX_AI + 1] Values
 *
 *	- Values[0] .. Values[ADDIDATA_MAX_AI-1] : Digital values of the initialized channels - each element is the next initialized channel
 *	- Values[ADDIDATA_MAX_AI] : Current counter
 *
 * @retval 0 Success
 * @retval 1 Internal error
 * @retval 2 Auto refresh not initialized
 */
#define CMD_xpci3xxx_ReadAnalogInputAutoRefreshValueAndCounter _IOR(xpci3xxx_MAGIC, 17,long)

/** Release (stop) an analog input auto refresh conversion.
 *
 * This function frees the resources allocated by a CMD_xpci3xxx_InitAnalogInputAutoRefresh
 * and must be called before initializing another acquisition.
 *
 * @param	NULL (ignored)
 *
 * @retval 0 : Success
 * @retval 1 : Internal error
 * @retval 2 : Auto refresh not initialized
 * @retval 3 : A conversion is already started
 */
#define CMD_xpci3xxx_ReleaseAnalogInputAutoRefresh _IOR(xpci3xxx_MAGIC, 18,long)
/** @} */

/* SEQUENCE */
/** \addtogroup sequence
* @{ */

/** Initialize an analog input sequence acquisition.
 *
 * This function prepare the board for an acquisition in sequence mode
 * and allocate the relevant resources. A call to CMD_xpci3xxx_ReleaseAnalogInputSequence
 * is necessary to release theses resources before being able to start another type of acquisition.
 *
 * @param[in]  uint32_t arg[((3*Number Of Channels) + 8)] Parameters
 *
 * arg[0]	Number of channel to initialize ( 1 .. ADDIDATA_MAX_AI)
 *
 * - arg[1] .. arg[arg[0]] : List of the channels to initialize
 * - arg[arg[0]+1] .. arg[2*arg[0]] : Gain factor
 *                                          	- ADDIDATA_1_GAIN : Gain =  1
 *                                          	- ADDIDATA_2_GAIN : Gain =  2
 *                                          	- ADDIDATA_5_GAIN : Gain =  5
 *                                          	- ADDIDATA_10_GAIN: Gain = 10
 * - arg[2*arg[0]+1] .. arg[3*arg[0]] : Polarity array of the channels
 *												- ADDIDATA_UNIPOLAR:  0V - +10V
 *												- ADDIDATA_BIPOLAR: -10V - +10V
 * - arg[3*arg[0]+1] : Hardware configuration = 0 :Single ended HW config  | 1 : Differential HW config
 * - arg[3*arg[0]+2] : if DMA not used:
 *                                          	- 0: continuous mode
 *                                          	- <>0: single mode if DMA used: define the number of DMA transfer
 * - arg[3*arg[0]+3] : DMA control = ADDIDATA_ENABLE | ADDIDATA_DISABLE
 * - arg[3*arg[0]+4] : number of sequence to acquire before generating an interrupt
 * - arg[3*arg[0]+5] : Delay mode for the analog input = ADDIDATA_DELAY_MODE_1  | ADDIDATA_DELAY_MODE_2
 * - arg[3*arg[0]+6] : Delay time unit for the analog input:
 *                                              -  ADDIDATA_MICRO_SECOND microseconds
 *                                              -  ADDIDATA_MILLI_SECOND milliseconds
 *                                              -  ADDIDATA_SECOND seconds
 * - arg[3*arg[0]+7] : Delay time value for the analog input conversion
 *
 *
 * @retval 0  Success
 * @retval 1  Internal error
 * @retval 2  Wrong number of channels
 * @retval 3  Wrong channel number
 * @retval 4  Wrong gain factor
 * @retval 5  Wrong polarity
 * @retval 6  Wrong single diff parameter (not 0 or 1)
 * @retval 7  Wrong use DMA parameter
 * @retval 8  Wrong number of sequence for each interrupt parameter
 * @retval 9  Wrong delay mode parameter
 * @retval 10 Wrong delay time unit parameter
 * @retval 11 Wrong delay time parameter
 * @retval 12 A conversion is already started
 * @retval 13 DMA mode not supported
 * @retval 14 No memory space available
 * @retval 15 Error by initialising the DMA
 * @retval 16 requested single diff parameter not supported by the board
 *
 * @note
 *
 * @section cop Constraints on parameter arg[0] (Number of channels)
 *
 * Let NumberOfAnalogInput be the number of analog inputs (4,8,16) of the board, then NumberOfChannel is correct if
 * - 0 < NumberOfChannel <= ADDIDATA_MAX_AI
 * - if the board is in differential mode, NumberOfChannel < (NumberOfAnalogInput/2)
 * - if the board is in single mode, NumberOfChannel < NumberOfAnalogInput
 *
 * @section mon More on List of Channel
 *
 * "List of Channel" describes the sequence to follow during acquisition. It must contain indexes of valid channels.
 * Which index is valid depends on the connection mode of the board (single or differential).
 *
 * The order of the index in this array is the order of the values returned after the acquisition.
 *
 * @section colci Constraints on the List of Channel to initialize
 *
 * Let NumberOfAnalogInput be the number of analog inputs (4,8,16) of the board, then "List of Channel to initialize" is correct if:
 * - if the board is in differencial mode, for each value in "List of Channel", value < (NumberOfAnalogInput/2)
 * - if the board is in single mode, for each value in "List of Channel", value < NumberOfAnalogInput
 *
 */
#define CMD_xpci3xxx_InitAnalogInputSequence _IOR(xpci3xxx_MAGIC, 20,long)

/** Start an analog input sequence acquisition.
 *
 * @param[in]   uint16_t arg[2] Parameters
 *
 *	- arg[0]	Convert time unit for the analog input:
 *                                              - ADDIDATA_NANO_SECOND : nanoseconds
 *                                              - ADDIDATA_MICRO_SECOND : microseconds
 *                                              - ADDIDATA_MILLI_SECOND : milliseconds
 *	- arg[1]   Convert time value for the analog input conversion
 *
 * @retval 0 Success
 * @retval 1 Internal error
 * @retval 2 Sequence not initialized
 * @retval 3 Wrong convert time unit
 * @retval 4 Wrong convert time
 * @retval 5 A conversion is already started
 * @retval 6 Error by starting DMA
 *
 * @note A sequence must have been initialized previously.
 */
#define CMD_xpci3xxx_StartAnalogInputSequence _IOR(xpci3xxx_MAGIC, 21,long)

/** Stop an analog input sequence acquisition.
 *
 * @param	NULL (ignored)
 *
 * @retval 0 Success
 * @retval 1 Internal error
 * @retval 2 Auto refresh not initialized
 * @retval 3 Sequence not started
 * @retval 4 Error by stoping DMA
 */
#define CMD_xpci3xxx_StopAnalogInputSequence _IOR(xpci3xxx_MAGIC, 22,long)

/** Release an analog input sequence conversion.
 *
 * This function frees the resources allocated by a CMD_xpci3xxx_InitAnalogInputSequence
 * and must be called before initializing another acquisition.
 *
 * @param	NULL (ignored)
 *
 * @retval 0 Success
 * @retval 1 Internal error
 * @retval 2 Auto refresh not initialized
 * @retval 3 A conversion is already started
 * @retval 4 Error by releasing DMA
 */
#define CMD_xpci3xxx_ReleaseAnalogInputSequence _IOR(xpci3xxx_MAGIC, 23,long)

/** @} */

/* ANALOG INPUT HARDWARE TRIGGER */

/** \addtogroup analogin
* @{ */

/** Analog Input Hardware Trigger control.
 *
 * Enable and initialize the analog input hardware trigger (if present)
 * or disable this trigger.
 *
 * @param[in]   uint32_t arg[4] Parameters
 *
 *	- arg[0] Trigger Control = 0 : disable the hardware trigger | 1 : enable the hardware trigger
 *	- arg[1] Hardware Trigger Level
 *		- ADDIDATA_LOW: If the hardware trigger is used, it triggers from "1" to "0"
 *		- ADDIDATA_HIGH: If the hardware trigger is used, it triggers from "0" to "1"
 *		- ADDIDATA_LOW_HIGH: If the hardware trigger is used, it triggers from "0" to "1" or from "1" to "0"
 *	- arg[2]   Action: defines how to react when the HW Trigger counter is reached
 *            			           	- ADDIDATA_TRIGGER_START_A_SINGLE_CONVERSION	0
 *            			            - ADDIDATA_ONE_SHOT_TRIGGER						1
 *            			            - ADDIDATA_TRIGGER_START_A_SEQUENCE_SERIES		2
 *            			            - ADDIDATA_TRIGGER_START_A_SINGLE_SEQUENCE		3
 *            			            - ADDIDATA_TRIGGER_START_A_SCAN_SERIES			6
 *            			            - ADDIDATA_TRIGGER_START_A_SINGLE_SCAN			7
 *            			            - ADDIDATA_TRIGGER_START_A_AUTO_REFRESH_SERIES	10
 *									- ADDIDATA_TRIGGER_START_A_SINGLE_AUTO_REFRESH	11
 *	- arg[3] Hardware trigger counter= Defines the number of trigger signals that must come before the "Action" occurs (> 0)
 *
 * @retval 0 Success
 * @retval 1 Internal error
 * @retval 2 Wrong hardware trigger parameter
 * @retval 3 Wrong hardware trigger level parameter
 * @retval 4 Wrong hardware trigger action parameter
 * @retval 5 Wrong hardware trigger count parameter
 * @retval 6 No analog input functionality available
 * @retval 7 A conversion is already started
 */
#define CMD_xpci3xxx_EnableDisableAnalogInputHardwareTrigger _IOR(xpci3xxx_MAGIC, 40,long)

/** Returns the current status of the trigger and the input state.
 *
 * @param[out] uint32_t arg[3] Results
 *
 * - arg[0]	Hardware Trigger Status = 0: Hardware trigger did not occur | 1: Hardware trigger did occur
 * - arg[1]	Hardware Trigger Count  : Number of triggering signals yet to happen before the "Action" occur.
 * - arg[2]   Hardware Trigger State  = 0: input not active (Low state) | 1: input active (High state)
 *
 * @retval 0 Success
 * @retval 1 Internal error
 * @retval 2 No analog input functionality available
 */
#define CMD_xpci3xxx_GetAnalogInputHardwareTriggerStatus _IOR(xpci3xxx_MAGIC, 41,long)

/* ANALOG INPUT SOFTWARE TRIGGER */

/** Control the Analog Input Software Trigger.
 *
 * Enable and initialize or disable the analog input software trigger.
 *
 * @param[in]	uint8_t arg[2]
 *
 * - arg[0]	Trigger Control = 0 : disable the software trigger | 1 : enable the software trigger
 * - arg[1] Action defines how to react when the SW Trigger counter is reached
 *            			            	- ADDIDATA_TRIGGER_START_A_SINGLE_CONVERSION	  0
 *            			            	- ADDIDATA_ONE_SHOT_TRIGGER					  1
 *            			            	- ADDIDATA_TRIGGER_START_A_SEQUENCE_SERIES	  2
 *            			            	- ADDIDATA_TRIGGER_START_A_SINGLE_SEQUENCE	  3
 *            			            	- ADDIDATA_TRIGGER_START_A_SCAN_SERIES	      6
 *            			            	- ADDIDATA_TRIGGER_START_A_SINGLE_SCAN  		  7
 *            			            	- ADDIDATA_TRIGGER_START_A_AUTO_REFRESH_SERIES 10
 *            			            	- ADDIDATA_TRIGGER_START_A_SINGLE_AUTO_REFRESH 11
 *
 * @retval 0 Success
 * @retval 1 Internal error
 * @retval 2 Wrong software trigger parameter
 * @retval 3 Wrong software trigger action parameter
 * @retval 4 No analog input functionality available
 * @retval 5 A conversion is already started
 */
#define CMD_xpci3xxx_EnableDisableAnalogInputSoftwareTrigger _IOR(xpci3xxx_MAGIC, 42,long)

/** Triggers the analog input acquisition.
 *
 *	@param	NULL (ignored)
 *
 * @retval 0 Success
 * @retval 1 Internal error
 * @retval 2 No analog input functionality available
 */
#define CMD_xpci3xxx_AnalogInputSoftwareTrigger _IOR(xpci3xxx_MAGIC, 43,long)

/** Returns the status of the software trigger.
 *
 * @param[out] uint8_t* arg = 0: Software trigger did not occur | 1: Software trigger occurred
 *
 * @retval 0 Success
 * @retval 1 Internal error
 * @retval 2 No analog input functionality available
 */

#define CMD_xpci3xxx_GetAnalogInputSoftwareTriggerStatus _IOR(xpci3xxx_MAGIC, 44,long)

/** Controls the analog input hardware gate.
 *
 * Enable and initialize, or disable, the analog input hardware gate.
 *
 * @param[in] uint8_t arg[2] Parameters
 *
 * - arg[0]	Hardware Gate Control = 0 : disable the hardware gate | 1 : enable the hardware gate
 * - arg[1]	Level
 * 				- ADDIDATA_LOW: If the hardware gate is used, it is active to "0"
 *				- ADDIDATA_HIGH:If the hardware gate is used, it is active to "1"
 *
 * @retval 0 Success
 * @retval 1 Internal error
 * @retval 2 Wrong hardware gate parameter
 * @retval 3 Wrong hardware gate level parameter
 * @retval 4 No analog input functionality available
 * @retval 5 A conversion is already started
 */
#define CMD_xpci3xxx_EnableDisableAnalogInputHardwareGate _IOR(xpci3xxx_MAGIC, 45,long)

/** Returns the state of the input (active or not)
 *
 * @param[out]  uint8_t* arg = 0: not active (Low state) | 1 : active (High state)
 *
 * @retval 0 Success
 * @retval 1 Internal error
 * @retval 2 No analog input functionality available
 */

#define CMD_xpci3xxx_GetAnalogInputHardwareGateStatus _IOR(xpci3xxx_MAGIC, 46,long)

/** @} */

/* ANALOG OUTPUTS */
/** \addtogroup analogout
* @{ */
/** Initializes one analog output channel
 *
 * @param[in]   uint8_t arg[2]
 *
 * - arg[0]	Channel number of the output to be initialized
 * - arg[1]	Voltage mode of the analog output
 *          - ADDIDATA_BIPOLAR: 0: -10V - +10V
 *          - ADDIDATA_UNIPOLAR:1: 0V - +10V
 *
 * @retval 0 : Success
 * @retval 1 : Internal error
 * @retval 2 : Wrong channel number
 * @retval 3 : Wrong voltage mode
 * @retval 4 : A conversion is already started
 */

#define CMD_xpci3xxx_InitAnalogOutput _IOR(xpci3xxx_MAGIC, 50,long)

/** Returns the EOC bit.
 *
 * Reads the EOC (End Of Conversion) bit of the analog output.
 * This bit is set by the hardware to indicate that a conversion is finished.
 *
 * @param[out] uint8_t *arg =  0: no analog output conversion finished | 1: analog output conversion finished
 *
 * @retval	0 : Success
 * @retval	1 : Internal error
 */
#define CMD_xpci3xxx_ReadAnalogOutputBit _IOR(xpci3xxx_MAGIC, 51,long)

/** Writes to the analog output channel.
 *
 * @param[in] uint32_t arg[2]
 *
 * - arg[0]	Channel number of the output to be initialized
 * - arg[1]	New value
 *
 * @retval	0 : Success
 * @retval	1 : Internal error
 * @retval	2 : Wrong channel number
 * @retval	3 : Wrong write value
 */
#define CMD_xpci3xxx_WriteAnalogOutputValue _IOR(xpci3xxx_MAGIC, 52,long)

/** @} */

/* DIGITAL INPUT FILTER */
/** \addtogroup digin
* @{ */

/** Read the digital input register of the board.
 *
 * The input range of the channels set to "1" is within the high voltage range
 * (See chapter 4 "Limit values" of the technical manual of the board concerned).
 * The input range of the channels set to "1" is within the low voltage range
 * (See chapter 4 "Limit values" of the technical manual of the board concerned).
 *
 * @param[out] uint32_t *arg Values of the digital input
 *
 * arg contains the value of the TTL and the 24V inputs (if the board supports them)
 *	- The values for the TTL are encoded in the 4 higher bytes (mask 0xFFFF0000)
 *	- The values for the 24V are encoded in the lower byte (mask 0x0000000F)
 *
 * @retval	0 : OK
 * @retval	1 : Internal error
 *
 */
#define CMD_xpci3xxx_Read32DigitalInputs _IOR(xpci3xxx_MAGIC, 60,long)

/** Initializes the digital input filter.
 *
 * @param[in]  uint32_t arg[2]
 *
 * - arg[0]	Control flag = 0: Disable | 1: Enable
 * - arg[1]	Reload value in microsecond units
 *
 * @retval	0 : OK
 * @retval	1 : Internal error
 * @retval	2 : Wrong filter flag
 * @retval	3 : Wrong filter reload value
 * @retval	4 : Digital input not available
 *
 * TODO: describe this filter
 */
#define CMD_xpci3xxx_InitDigitalInputModuleFilter _IOR(xpci3xxx_MAGIC, 61,long)

/** @} */

/* DIGITAL OUTPUTS */
/** \addtogroup digout
* @{ */

/** Actives digital output memory on.
 *
 * This command modifies the behavior of the commands Set32DigitalOutputsOn and Set32DigitalOutputsOff.
 *
 * @param	NULL (ignored)
 *
 * @retval	0 : OK
 * @retval	1 : Internal error
 */
#define CMD_xpci3xxx_SetDigitalOutputMemoryOn _IOR(xpci3xxx_MAGIC, 72,long)

/** Inactives the digital output memory
 *
 * This command modifies the behavior of the commands Set32DigitalOutputsOn and Set32DigitalOutputsOff.
 *
 * @param	NULL (ignored)
 *
 *
 * @retval	0 : OK
 * @retval	1 : Internal error
 */
#define CMD_xpci3xxx_SetDigitalOutputMemoryOff _IOR(xpci3xxx_MAGIC, 73,long)

/** Set digital outputs of the board.
 *
 * Sets one or several outputs of a port.
 * Setting one output means setting the output to "High".
 *
 * If you have activated the digital output memory, the selected outputs channels
 * are set to "1". The other channels hold their state.
 *
 * If the digital output memory is not active, the selected outputs are set
 * to "1". The other channels are reset to "0". This correspond to directly writing the output state.
 *
 * @param[in] uint32_t *arg	Digital output value
 *
 * The parameter sets the TTL and the 24V outputs (if the board support them)
 *	- The values for the TTL are encoded in the 4 higher bytes (mask 0xFFFF0000)
 *	- The values for the 24V are encoded in the lower byte (mask 0x0000000F)
 *
 * @retval	0 : OK
 * @retval	1 : Internal error
 */
#define CMD_xpci3xxx_Set32DigitalOutputsOn _IOR(xpci3xxx_MAGIC, 70,long)

/** Set digital outputs from the board off.
 *
 * Resetting an output means setting this output to "Low".
 * This command can only be used when the digital output memory is active.
 *
 * @param[in] uint32_t *arg	Bit mask
 *
 * Bits that are set in the mask are cleared and the corresponding channels set to off.
 * Bits not set are ignored, and the original value not changed.
 *
 * The parameter sets the TTL and the 24V outputs (if the board support them)
 *	- The values for the TTL are encoded in the 4 higher bytes (mask 0xFFFF0000)
 *	- The values for the 24V are encoded in the lower byte (mask 0x0000000F)
 *
 * @retval 0 : OK
 * @retval 1 : Internal error
 * @retval 2 : Digital output memory not active
 *
 * @note if digital output memory is not active, you want to use
 * CMD_xpci3xxx_Set32DigitalOutputsOn to set the output state.
 *
 */
#define CMD_xpci3xxx_Set32DigitalOutputsOff _IOR(xpci3xxx_MAGIC, 71,long)

/** Get the digital output current status.
 *
 * This command allows to retrieve the current state of the outputs as known by the board.
 * Each bit set means the corresponding channel is on.
 *
 * @param[out] uint32_t *arg	Digital output status
 *
 * This value encodes the TTL and the 24V outputs (if the board support them)
 * The values for the TTL are encoded in the 4 higher bytes (mask 0xFFFF0000)
 * The values for the 24V are encoded in the lower byte (mask 0x0000000F)
 *
 * @retval 0 OK
 * @retval 1 Internal error
 */
#define CMD_xpci3xxx_Get32DigitalOutputStatus _IOR(xpci3xxx_MAGIC, 74,long)

/** @} */

/* TIMERS */
/** \addtogroup timer
* @{ */

/** Initialize the timer.
 *
 * @param[in] uint32_t arg[4] Parameters
 *
 *	- arg[0]	Timer number
 *	- arg[1]	Timer mode (see below)
 *	- arg[2]	Timer Time Unit	: Timer unit
 *					- 0 : nanoseconds
 *					- 1 : microseconds
 *					- 2 : milliseconds
 *					- 3 : seconds
 *	- arg[3]	Start value or time interval (depending of the board)
 *
 * @retval 0 : Success
 * @retval 1 : Internal error
 * @retval 2 : Wrong Timer number
 * @retval 3 : Timer already used as counter
 * @retval 4 : Timer already used as watchdog
 * @retval 5 : Wrong Timer mode
 * @retval 6 : Wrong Timer time unit
 * @retval 7 : Wrong reload value
 *
 */

/**
 * @section tmd Timer mode description
 *
@verbatim

Selected                     Mode description                   dw_ReloadValue  Hardware gate
 mode                                                              description   input action
 0       Mode 0 is typically used for event counting. After    Start counting  Hardware gate
         the initialization, OUT is initially low, and remains     value
         low until the counter reaches 0. OUT goes high and
         remains high until a new count is written.
 1       Mode 1 is similar to mode 0 except for the gate       Start counting    Hardware
         input action.                                             value           trigger
         The gate input is not used to enable or disable the
         timer (like in Mode 0), but to trigger it.
 2       This mode functions like a divide-by-                 Divider factor  Hardware gate
         ul_ReloadValue counter. It is used to generate a real
         time clock interrupt. OUT is initially high after the
         initialization. When the initial count has
         decremented to 1, OUT goes low for one CLK pulse.
         OUT then goes high again, the counter reloads the
         initial count (ul_ReloadValue) and the process is
         repeated. This action can generate an interrupt.
 3       Mode 3 is typically used for baud rate generation.    Divider factor  Hardware gate
         This mode is similar to mode 2 except for the duty
         cycle of OUT. OUT will initially be high after the
         initialization. When the initial count
         (ul_ReloadValue) has expired, OUT goes low for
         the next sequence.then the initial count (ul_ReloadValue)
         of that sequence has expired, OUT goes high again for
         the following sequence.
         The mode is periodic;
         the sequence above is repeated indefinitely.
 4       OUT is initially high after the initialization. When  Start counting  Hardware gate
         the initial count expires, OUT will go low for one        value
         CLK pulse and then go high again. The counting
         sequence is triggered by writing a new value. If a
         new count is written during counting, it will be
         loaded on the next CLK pulse.
 5       Mode 5 is similar to mode 4 except for the gate input Start counting    Hardware
         action.                                                   value           trigger
         The gate input is not used to enable or disable the
         timer, but to trigger it.
@endverbatim
*/
#define CMD_xpci3xxx_InitTimer _IOR(xpci3xxx_MAGIC, 80,long)

/** Stop and free the timer.
 *
 * If a counter has been initialized, it can be reinitialized again
 * only if this command is called before.
 *
 * @param[in] uint8_t *arg Select the timer

 * @retval 0  : Success
 * @retval 1 : Internal error
 * @retval 2 : Timer not initialized
 * @retval 3 : Wrong Timer number
 * @retval 4 : Timer is configured as counter
 * @retval 5 : Timer is configured as watchdog
 *
 */
#define CMD_xpci3xxx_ReleaseTimer _IOR(xpci3xxx_MAGIC, 81,long)

/** Start selected timer.
 *
 * @param[in] uint8_t *arg Selected timer number
 *
 * @retval 0 : Success
 * @retval 1 : Internal error
 * @retval 2 : Timer not initialized
 * @retval 3 : Wrong Timer number
 * @retval 4 : Timer was initialized as counter
 * @retval 5 : Timer was initialized as watchdog
 *
 */
#define CMD_xpci3xxx_StartTimer _IOR(xpci3xxx_MAGIC, 82,long)

/** Start all already initialized timers
 *
 * @param	NULL (ignored)
 *
 * @retval 0  Success
 * @retval 1 Internal error
 *
 */
#define CMD_xpci3xxx_StartAllTimers _IOR(xpci3xxx_MAGIC, 83,long)

/** Triggers the selected timer.
 *
 * @param[in] uint8_t *arg Selected timer number
 *
 * @retval 0  : Success
 * @retval 1 : Internal error
 * @retval 2 : Timer not initialized
 * @retval 3 : Wrong Timer number
 * @retval 4 : Timer was initialized as counter
 * @retval 5 : Timer was initialized as watchdog
 *
 */
#define CMD_xpci3xxx_TriggerTimer _IOR(xpci3xxx_MAGIC, 84,long)

/** Triggers all initialized timers.
 *
 * @param	NULL (ignored)
 *
 * @retval 0  : Success
 * @retval 1 : Internal error
 *
 */
#define CMD_xpci3xxx_TriggerAllTimers _IOR(xpci3xxx_MAGIC, 85,long)

/** Stop the selected timer.
 *
 * @param[in] uint8_t *arg Selected timer number
 *
 * @retval 0  : Success
 * @retval 1 : Internal error
 * @retval 2 : Timer not initialized
 * @retval 3 : Wrong Timer number
 * @retval 4 : Timer was initialized as counter
 * @retval 5 : Timer was initialized as watchdog
 *
 */
#define CMD_xpci3xxx_StopTimer _IOR(xpci3xxx_MAGIC, 86,long)


/** Stop all started timers.
 *
 * @param	NULL (ignored)
 *
 * @retval 0 : Success
 * @retval 1 : Internal error
 *
 */
#define CMD_xpci3xxx_StopAllTimers _IOR(xpci3xxx_MAGIC, 87,long)


/** Read the timer value.
 *
 * @param[in,out] uint32_t arg[2] Parameters and result
 *
 * - arg[0] (in) Selected timer number - indexed from 0
 * - arg[1] (out) current timer value
 *
 * @retval 0  : Success
 * @retval 1 : Internal error
 * @retval 2 : Timer not initialized
 * @retval 3 : Wrong Timer number
 * @retval 4 : Timer was initialized as counter
 * @retval 5 : Timer was initialized as watchdog
 *
 */
#define CMD_xpci3xxx_ReadTimerValue _IOR(xpci3xxx_MAGIC, 88,long)

/** Returns the timer status.
 *
 * @param[in,out] uint8_t b_ArgTable[4] Parameter and results
 *
 * - arg[0] (in) Selected timer number
 * - arg[1] (out) Timer status =  0 : no overflow | 1 : timer overflow
 * - arg[2] (out) Software Trigger Status = 0 : none | 1 : software trigger occurred
 * - arg[3] (out) Hardware Trigger Status = 0 : none | 1 : hardware trigger occurred
 *
 * @note When the status of the software trigger is read, it is automatically reset.
 * At the next call of this command, 0 is returned if no trigger occurred during this period.
 *
 * @retval 0  : Success
 * @retval 1 : Internal error
 * @retval 2 : Timer not initialized
 * @retval 3 : Wrong Timer number
 * @retval 4 : Timer was initialized as counter
 * @retval 5 : Timer was initialized as watchdog
 *
 */
#define CMD_xpci3xxx_ReadTimerStatus _IOR(xpci3xxx_MAGIC, 89,long)

/** Enable/disable the timer interrupt
 *
 * Controls if a timer will or not generate an interrupt.
 *
 * @param[in] uint8_t *arg Parameters
 *
 * - arg[0]	Timer number
 * - arg[1]	Control flag = ADDIDATA_ENABLE | ADDIDATA_DISABLE
 *
 * @retval 0  : Success
 * @retval 1 : Internal error
 * @retval 2 : Timer not initialized
 * @retval 3 : Wrong Timer number
 * @retval 4 : Timer was initialized as counter
 * @retval 5 : Timer was initialized as watchdog
 * @retval 6 : Wrong control flag
 * @retval 7 : Interrupt call-back not installed
 */
#define CMD_xpci3xxx_EnableDisableTimerInterrupt _IOR(xpci3xxx_MAGIC, 90,long)

/** Enable/disable the hardware timer output.
 *
 * @param[in] uint8_t arg[3] Parameters
 *
 * - arg[0]	Selected timer number
 * - arg[1]	Control flag = ADDIDATA_DISABLE : not used | ADDIDATA_ENABLE : active
 * - arg[2]	output Level
 * 				- ADDIDATA_HIGH : when counter overflows, the output is set to high
 *				- ADDIDATA_LOW  : If the counter overflows, the output is set to low
 *
 * @retval 0  : Success
 * @retval 1 : Internal error
 * @retval 2 : Timer not initialized
 * @retval 3 : Wrong Timer number
 * @retval 4 : Timer was initialized as counter
 * @retval 5 : Timer was initialized as watchdog
 * @retval 6 : Wrong output flag
 * @retval 7 : Wrong output level
 *
 */
#define CMD_xpci3xxx_EnableDisableTimerHardwareOutput _IOR(xpci3xxx_MAGIC, 91,long)

/** Returns the status of the output of the hardware timer.
 *
 * @param[in,out] uint8_t *arg Parameter and result
 *
 * - arg[0] (in) Selected timer number
 * - arg[1] (out)	Status = 0 : output not active | 1: output is active
 *
 * @retval 0 : Success
 * @retval 1 : Internal error
 * @retval 2 : Timer not initialized
 * @retval 3 : Wrong Timer number
 * @retval 4 : Timer was initialized as counter
 * @retval 5 : Timer was initialized as watchdog
 *
 */
#define CMD_xpci3xxx_GetTimerHardwareOutputStatus _IOR(xpci3xxx_MAGIC, 92,long)

/** @} */

/* COUNTERS */
/** \addtogroup counter
* @{ */

/** Initialize the counter.
 *
 * @param[in] uint32_t arg[4]	Parameters
 *
 * - arg[0]	Selected counter number
 * - arg[1]	Direction Selection = ADDIDATA_UP | ADDIDATA_DOWN
 * - arg[2]	Level Selection :
 *				- ADDIDATA_LOW : Counter counts each low level
 *				- ADDIDATA_HIGH : Counter count each high level
 *				- ADDIDATA_LOW_HIGH Counter count each high and low level
 * - arg[3] Reload value
 *
 * @retval 0  : Success
 * @retval 1 : Internal error
 * @retval 2 : Wrong Counter number
 * @retval 3 : Counter as timer already used
 * @retval 4 : Counter as watchdog already used
 * @retval 5 : Wrong direction selection
 * @retval 6 : Wrong level selection
 * @retval 7 : Wrong reload value
 */
#define CMD_xpci3xxx_InitCounter _IOR(xpci3xxx_MAGIC, 100,long)



/** Stops and frees the counter.
*
* If a counter has been initialized, it can be reinitialized again
* only if this command is called before.
*
* @param[in]  uint8_t *b_CounterNumber Selected counter number
*
* @retval 0 : Success
* @retval 1 : Internal error
* @retval 2 : Counter not intialised
* @retval 3 : Wrong counter number
* @retval 4 : Counter as timer already used
* @retval 5 : Counter as Watchdog already used
*/
#define CMD_xpci3xxx_ReleaseCounter _IOR(xpci3xxx_MAGIC, 101,long)

/** Start the selected counter.
*
* @param[in]   uint8_t *arg Selected counter number
*
* @retval 0 : Success
* @retval 1 : Internal error
* @retval 2 : Counter not intialised
* @retval 3 : Wrong counter number
* @retval 4 : Counter as timer already used
* @retval 5 : Counter as Watchdog already used
*/
#define CMD_xpci3xxx_StartCounter _IOR(xpci3xxx_MAGIC, 102,long)

/** Start all initialized counters.
*
* @param	NULL (ignored)
*
* @retval 0  : Success
* @retval 1 : Internal error
*/
#define CMD_xpci3xxx_StartAllCounters _IOR(xpci3xxx_MAGIC, 103,long)


/** Triggers the selected counter.
*
* @param[in]  uint8_t *arg  : Selected counter number
*
* @retval 0  : Success
* @retval 1 : Internal error
* @retval 2 : Counter not intialised
* @retval 3 : Wrong counter number
* @retval 4 : Counter as timer already used
* @retval 5 : Counter as Watchdog already used
*/
#define CMD_xpci3xxx_TriggerCounter _IOR(xpci3xxx_MAGIC, 104,long)

/** Trigger all counters.
*
* @param	NULL (ignored)
*
* @retval 0 : Success
* @retval 1 : Internal error
*/
#define CMD_xpci3xxx_TriggerAllCounters _IOR(xpci3xxx_MAGIC, 105,long)

/** Stop the selected counter.
*
* @param[in]	uint8_t *arg Selected counter number
*
* @retval 0  : Success
* @retval 1 : Internal error
* @retval 2 : Counter not intialised
* @retval 3 : Wrong counter number
* @retval 4 : Counter as timer already used
* @retval 5 : Counter as Watchdog already used
*/
#define CMD_xpci3xxx_StopCounter _IOR(xpci3xxx_MAGIC, 106,long)

/** Stop all running counters.
*
* @param	NULL (ignored)
*
* @retval 0 : Success
* @retval 1 : Internal error
*/
#define CMD_xpci3xxx_StopAllCounters _IOR(xpci3xxx_MAGIC, 107,long)

/** Clear the selected counter.
*

* @param[in]  uint8_t *arg	Selected counter number
*
* @retval 0 : Success
* @retval 1 : Internal error
* @retval 2 : Counter not intialised
* @retval 3 : Wrong counter number
* @retval 4 : Counter as timer already used
* @retval 5 : Counter as Watchdog already used
*/
#define CMD_xpci3xxx_ClearCounter _IOR(xpci3xxx_MAGIC, 108,long)

/** Read the counters value.
*
* @param[in,out] uint32_t arg[2] Parameter and result
*
* - arg[0]	(in) Selected counter number
* - arg[1]	(out) Current counter value
*
* @retval 0  : Success
* @retval 1 : Internal error
* @retval 2 : Counter not intialised
* @retval 3 : Wrong counter number
* @retval 4 : Counter as timer already used
* @retval 5 : Counter as Watchdog already used
*/
#define CMD_xpci3xxx_ReadCounterValue _IOR(xpci3xxx_MAGIC, 109,long)


/** Returns counter status.
*
* @param[in,out]  uint8_t arg[5] Parameter and result
*
* - arg[0]	Selected counter number
* - arg[1]	Overflow status = 0 : none | 1 : overflow occurred
* - arg[2]	Software Trigger Status = 0 : none, 1 : sw trigger occurred
* - arg[3]	Hardware Trigger Status = 0 : none, 1 : hw trigger occurred
* - arg[4]	Software clear status = 0 : none, 1 : sw clear occurred
*
* @retval 0  : Success
* @retval 1 : Internal error
* @retval 2 : Counter not intialised
* @retval 3 : Wrong counter number
* @retval 4 : Counter as timer already used
* @retval 5 : Counter as Watchdog already used
*/
#define CMD_xpci3xxx_ReadCounterStatus _IOR(xpci3xxx_MAGIC, 110,long)

/** Enable/disable the counter interrupt.
*
* @param[in]	uint8_t arg[2] Parameters
*
* - arg[0]	Selected counter number
* - arg[1]	Interrupt flag = ADDIDATA_DISABLE | ADDIDATA_ENABLE
*
* @retval 0 : Success
* @retval 1 : Internal error
* @retval 2 : Counter not initialized
* @retval 3 : Wrong counter number
* @retval 4 : Counter as timer already used
* @retval 5 : Counter as Watchdog already used
* @retval 6 : Wrong interrupt flag
* @retval 7 : Interrupt call-back not installed
*/
#define CMD_xpci3xxx_EnableDisableCounterInterrupt _IOR(xpci3xxx_MAGIC, 111,long)

/** Enable/disable the hardware counter output.
*
* @param[in] uint8_t arg[3] Parameters
*
* - arg[0]	Counter number
* - arg[1]	Control flag = ADDIDATA_DISABLE | ADDIDATA_ENABLE
* - arg[2]	Output Level
* 				- ADDIDATA_HIGH : If the counter counter overflows, the output is set to high
*				- ADDIDATA_LOW  : If the counter counter overflows, the output is set to low
*
* @retval 0  : Success
* @retval 1 : Internal error
* @retval 2 : Counter not intialised
* @retval 3 : Wrong Counter number
* @retval 4 : Counter as timer already used
* @retval 5 : Counter as watchdog already used
* @retval 6 : Wrong output flag
* @retval 7 : Wrong output level
*/
#define CMD_xpci3xxx_EnableDisableCounterHardwareOutput _IOR(xpci3xxx_MAGIC, 112,long)

/** Return the status of the hardware counter output.
*
* @param[in,out]	 uint8_t arg[2] Parameter and result
*
*	- arg[0]	Selected counter number
*	- arg[1]	Status = 0 : not active | 1 : active
*
* @retval 0  : Success
* @retval 1 : Internal error
* @retval 2 : Counter not intialised
* @retval 3 : Wrong Counter number
* @retval 4 : Counter as timer already used
* @retval 5 : Counter as watchdog already used
*/
#define CMD_xpci3xxx_GetCounterHardwareOutputStatus _IOR(xpci3xxx_MAGIC, 113,long)

/** @} */
/* WATCHDOG */
/** \addtogroup watchdog
* @{ */

/** Initialize the watchdog.
 *
 * @param[in]	uint32_t *arg	Parameters
 *
 *	- arg[0]	Selected watchdog number
 *	- arg[1]	Watchdog Time Unit = 0 : nanosecond | 1 : microsecond | 2 : millisecond | 3 : second
 *	- arg[2]	Watchdog reload value
 *
 * @retval 0  : Success
 * @retval 1 : Internal error
 * @retval 2 : Wrong watchdog number
 * @retval 3 : Watchdog already configured as timer
 * @retval 4 : Watchdog already used as counter
 * @retval 5 : Wrong time unit
 * @retval 6 : Wrong reload value
 */
#define CMD_xpci3xxx_InitWatchdog _IOR(xpci3xxx_MAGIC, 120,long)

/** Stops and frees the watchdog.
 *
 * @param[in]	uint8_t *arg Selected watchdog number
 *
 * @retval 0 : Success
 * @retval 1 : Internal error
 * @retval 2 : Watchdog not intialised
 * @retval 3 : Wrong watchdog number
 * @retval 4 : Watchdog already configured as timer
 * @retval 5 : Watchdog already used as counter
 */
#define CMD_xpci3xxx_ReleaseWatchdog _IOR(xpci3xxx_MAGIC, 121,long)

/** Start the selected watchdog.
 *
 * @param[in]	uint8_t *arg Selected watchdog number
 *
 * @retval 0 : Success
 * @retval 1 : Internal error
 * @retval 2 : Watchdog not intialised
 * @retval 3 : Wrong watchdog number
 * @retval 4 : Watchdog already configured as timer
 * @retval 5 : Watchdog already used as counter
 */
#define CMD_xpci3xxx_StartWatchdog _IOR(xpci3xxx_MAGIC, 122,long)


/** Start all initialized watchdogs.
 *
 * @param NULL (ignored)
 *
 * @retval 0  : Success
 * @retval 1 : Internal error
 */
#define CMD_xpci3xxx_StartAllWatchdogs _IOR(xpci3xxx_MAGIC, 123,long)

/** Triggers the selected watchdog.
 *
 * @param[in]	uint8_t *arg Selected watchdog number
 *
 * @retval 0 : Success
 * @retval 1 : Internal error
 * @retval 2 : Watchdog not intialised
 * @retval 3 : Wrong watchdog number
 * @retval 4 : Watchdog already configured as timer
 * @retval 5 : Watchdog already used as counter
 */
#define CMD_xpci3xxx_TriggerWatchdog _IOR(xpci3xxx_MAGIC, 124,long)

/** Triggers all watchdogs.
 *
 * @param NULL (ignored)
 *
 * @retval 0  : Success
 * @retval 1 : Internal error
 */
#define CMD_xpci3xxx_TriggerAllWatchdogs _IOR(xpci3xxx_MAGIC, 125,long)


/** Stop the selected watchdog.
 *
 * @param[in]   uint8_t *arg  Selected watchdog number
 *
 * @retval 0  : Success
 * @retval 1 : Internal error
 * @retval 2 : Watchdog not intialised
 * @retval 3 : Wrong watchdog number
 * @retval 4 : Watchdog already configured as timer
 * @retval 5 : Watchdog already used as counter
 */
#define CMD_xpci3xxx_StopWatchdog _IOR(xpci3xxx_MAGIC, 126,long)

/** Stop all watchdogs.
 *
 * @param NULL (ignored)
 *
 * @retval 0 : Success
 * @retval 1 : Internal error
 */
#define CMD_xpci3xxx_StopAllWatchdogs _IOR(xpci3xxx_MAGIC, 127,long)

/** Read the watchdogs status.
 *
 * @param[in,out]   uint8_t arg[4] Parameter and result
 *
 * - arg[0]	Selected watchdog number
 * - arg[1]	Overflow status = 0 : none | 1 : occurred
 * - arg[2]	Software Trigger Status = 0 : none | 1 : occurred
 * - arg[3]	Hardware Trigger Status = 0 : none | 1 : occurred
 *
 * @retval 0 : Success
 * @retval 1 : Internal error
 * @retval 2 : Watchdog not initialized
 * @retval 3 : Wrong watchdog number
 * @retval 4 : Watchdog already configured as timer
 * @retval 5 : Watchdog already used as counter
 */
#define CMD_xpci3xxx_ReadWatchdogStatus _IOR(xpci3xxx_MAGIC, 128,long)

/** Read the watchdog value.
 *
 * @param[in,out] uint32_t arg[2]
 *
 * - arg[0] (in) Selected watchdog number
 * - arg[1] Current watchdog value
 *
 * @retval 0  : Success
 * @retval 1 : Internal error
 * @retval 2 : Watchdog not intialised
 * @retval 3 : Wrong watchdog number
 * @retval 4 : Watchdog already configured as timer
 * @retval 5 : Watchdog already used as counter
 */
#define CMD_xpci3xxx_ReadWatchdogValue _IOR(xpci3xxx_MAGIC, 129,long)

/** Enable/disable the watchdog interrupt.
 *
 * uint8_t arg[2] Parameters
 *
 * - arg[0]	Selected watchdog number
 * - arg[1]	Control flag = ADDIDATA_DISABLE | ADDIDATA_ENABLE
 *
 *
 * @retval 0  : Success
 * @retval 1 : Internal error
 * @retval 2 : Watchdog not intialised
 * @retval 3 : Wrong watchdog number
 * @retval 4 : Watchdog already configured as timer
 * @retval 5 : Watchdog already used as counter
 * @retval 6 : Wrong interrupt flag
 * @retval 7 : Interrupt Callback not installed
 */
#define CMD_xpci3xxx_EnableDisableWatchdogInterrupt _IOR(xpci3xxx_MAGIC, 130,long)

/** Enable/disable the watchdog hardware output.
 *
 *
 * @param[in]   uint8_t arg[3]	Parameters
 *
 * - arg[0]	Selected watchdog number
 * - arg[1]   Control flag = ADDIDATA_DISABLE | ADDIDATA_ENABLE
 * - arg[3]	Output level on watchdog overflow = ADDIDATA_HIGH : output to high | ADDIDATA_LOW  : output to low
 *
 * @retval 0 : Success
 * @retval 1 : Internal error
 * @retval 2 : Watchdog not intialised
 * @retval 3 : Wrong Watchdog number
 * @retval 4 : Watchdog already configured as timer
 * @retval 5 : Watchdog already used as counter
 * @retval 6 : Wrong output flag
 * @retval 7 : Wrong output level
 */
#define CMD_xpci3xxx_EnableDisableWatchdogHardwareOutput _IOR(xpci3xxx_MAGIC, 131,long)

/** Return the status of the hardware watchdog output.
 *
 * @param[in,out]	uint8_t arg[2]
 *
 * - arg[0]	(in ) Selected watchdog number
 * - arg[1]	(out) Hardware output status = 0 : not activate | 1 : activate
 *
 * @retval	0  : No error occur
 * @retval	1 : Internal error
 * @retval	2 : Watchdog not intialised
 * @retval	3 : Wrong Watchdog number
 * @retval	4 : Watchdog as timer already used
 * @retval	5 : Watchdog already used as counter
 */
#define CMD_xpci3xxx_GetWatchdogHardwareOutputStatus _IOR(xpci3xxx_MAGIC, 132,long)

/** Enable / Disable the timer hardware trigger
 *
 * @param[in,out]	uint8_t arg[3]
 *
 * - arg[0] (in): Selected timer index (at that time only timer 0).
 * - arg[1] (in): 0: Disable / 1: Enable the hardware trigger.
 * - arg[2] (in): Trigger level\n
 *            - 1: Trigger on low level.\n
 *            - 2: Trigger on high level.\n
 *            - 3: Trigger on low and high levels.\n
 *
 * @retval 0 : No error occur
 * @retval 1 : The handle parameter of the board is wrong
 * @retval 2 : Timer not initialized
 * @retval 3 : Wrong Timer number
 * @retval 4 : Timer as counter already used
 * @retval 5 : Timer as watchdog already used
 */
#define CMD_xpci3xxx_EnableDisableTimerHardwareTrigger _IOR(xpci3xxx_MAGIC, 133,long)

#define CMD_xpci3xxx_OUTPORTDW _IOWR(xpci3xxx_MAGIC, 134,long)
#define CMD_xpci3xxx_INPORTDW _IOWR(xpci3xxx_MAGIC, 135,long)

/** @} */

/** Used internally. This is the ioctl CMD with the highest number.
* @internal
*/
#define __xpci3xxx_UPPER_IOCTL_CMD 135


/** analog input sequence begin at (start) channel index - mask
 * @internal
 */
#define ANALOG_SEQUENCE_START_BITMASK 0x0000000F
/** analog input sequence terminate (stop) at channel index - mask */
#define ANALOG_SEQUENCE_STOP_BITMASK	0x00000F00

#define ADDIDATA_TRIGGER_START_A_SINGLE_CONVERSION		0
#define ADDIDATA_ONE_SHOT_TRIGGER						1
#define ADDIDATA_TRIGGER_START_A_SEQUENCE_SERIES		2
#define ADDIDATA_TRIGGER_START_A_SINGLE_SEQUENCE		3
#define ADDIDATA_TRIGGER_START_A_SCAN_SERIES			6
#define ADDIDATA_TRIGGER_START_A_SINGLE_SCAN			7
#define ADDIDATA_TRIGGER_START_A_AUTO_REFRESH_SERIES 	10
#define ADDIDATA_TRIGGER_START_A_SINGLE_AUTO_REFRESH 	11

#define ADDIDATA_MAX_INTERRUPT_ANALOG_INPUT_VALUE		100

/**< size of the event FIFO
 * @internal
 * */
#define ADDIDATA_MAX_EVENT_COUNTER	256

#endif // __xpci3xxx_H__
