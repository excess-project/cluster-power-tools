/** @file sample_read_1_analog_input.c
*
* @author Krauth Julien
*
* @date 28.04.2008
*
* @version  $LastChangedRevision$
* @version $LastChangedDate$
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


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/sysmacros.h> /* major() minor() */
#include <signal.h>
#include <errno.h>
#include <limits.h>

#include <xpci3xxx.h>

#include "errorlib.h"
#include "findboards.h"
#include "errormsg.h"
#include "input.h"

//--------------------------------------------------------------------------------
/** this function look up for cards in the /proc/sys/xpci3xxx/ directory
* @param cards A pointer to the card area to fill
* @return The number of cards detected (may be zero if nor cards found).
*/
int xpci3xxx_find_cards(int ** cards)
{
	return apci_find_boards("xpci3xxx", cards);
}

//--------------------------------------------------------------------------------


/** This read_1_analog_input function mask the driver function to read 1 channel value.
 *  In fact the driver doesn't include a single function to read
 *  1 channel. The function to read 1 channel is splitted into
 *  4 functions. This permit to use the board in realtime environment.
 *
 * @param [in] fd				The file descriptor of the board to use.
 * @param [in] channel		The channel to read (0 to x).
 * @param [in] mode			Single = 0, differential = 1.
 * @param [in] gain			- ADDIDATA_1_GAIN : Gain =  1
 *                      	- ADDIDATA_2_GAIN : Gain =  2
 *                      	- ADDIDATA_5_GAIN : Gain =  5
 *                      	- ADDIDATA_10_GAIN: Gain = 10
 * @param [in] polarity		Polarity of the analog input
 *                         - ADDIDATA_UNIPOLAR:  0V to +10V
 *                         - ADDIDATA_BIPOLAR: -10V to +10V
 * @param [in] conversion_time	Time used by the chip to convert the analog value in digital value.
 * @param [out] value		Read analog value.
 *
 *	@retval 0 No error.
 *	@retval 1 ADDI-DATA driver error.
 *	@retval 2 IOCTL error.
 */
int read_1_analog_input(int fd, unsigned long channel, unsigned long mode, unsigned long gain, unsigned long polarity, unsigned long conversion_time, unsigned long *value)
{

	int ret = 0;
	uint16_t arg_w_array[5] = {0};
	uint8_t arg_b_array[5] = {0};
	uint8_t arg_b = 0;
	uint32_t arg_dw = 0;

	// Prepare all parameters
	// Parameters are given by using an array
	arg_b_array[0] = channel;
	arg_b_array[1] = mode;
	arg_b_array[2] = gain;
	arg_b_array[3] = polarity;

	// This is an IOCTL call to execute the InitAnalogInput function
	ret = ioctl (fd, CMD_xpci3xxx_InitAnalogInput, arg_b_array);

	// The return value has to be tested to see if all is OK
	// A return value "ret" < 0 is an system error
	// A return value "ret" > 0 is an error of the driver
	// "ret" = 0 is no success
	if (ret < 0)
	{
		perror("ioctl");
		return 2;
	}
	else
	{
		if (ret > 0)
		{
			printf(" CMD_xpci3xxx_InitAnalogInput error: %d\n", ret);
			return 1;
		}
	}

	// The read 1 analog function is diveded in 3 functions.
	// At a first look to read 1 analog input seems to be compilcate
	// but such functions permits to write fast code and integrate
	// it in realtime applications.

	// Prepare all parameters
	// Parameters are given by using an array
	arg_w_array[0] = channel;
	arg_w_array[1] = ADDIDATA_MICRO_SECOND;		// The unit for the conversion time
	arg_w_array[2] = conversion_time; 				// The conversion time is of 10 us

	// The conversion time is the time that is needed by the board to convert the voltage into a digital value

	// This is an IOCTL call to execute the StartAnalogInput function
	ret = ioctl (fd, CMD_xpci3xxx_StartAnalogInput, arg_w_array);

	// The return value has to be tested to see if all is OK
	// A return value "ret" < 0 is an system error
	// A return value "ret" > 0 is an error of the driver
	// "ret" = 0 is no error
	if (ret < 0)
	{
		perror("ioctl");
		return 2;
	}
	else
	{
		if (ret > 0)
		{
			printf("CMD_xpci3xxx_StartAnalogInput error: %d\n", ret);
			return 1;
		}
	}

	// The channel is initialised and the conversion is started,
	// now we have to wait that the board finished to convert
	// the voltage value into a digital value. When the board complete
	// this operation, it set a flag. We have to test this flag.

	do
	{
		ret = ioctl (fd, CMD_xpci3xxx_ReadAnalogInputBit, &arg_b);

		// The return value has to be tested to see if all is OK
		// A return value "ret" < 0 is an system error
		// A return value "ret" > 0 is an error of the driver
		// "ret" = 0 is no error
		if (ret < 0)
		{
			perror("ioctl");
			return 2;
		}
		else
		{
			if (ret > 0)
			{
				printf("CMD_xpci3xxx_ReadAnalogInputBit error: %d\n", ret);
				return 1;
			}
		}
	}
	while (arg_b != 1); // Test if the conversion is finished


	// The conversion is finished, we can read the value!

	// This is an IOCTL call to execute the StartAnalogInput function
	ret = ioctl (fd, CMD_xpci3xxx_ReadAnalogInputValue, &arg_dw);

	// The return value has to be tested to see if all is OK
	// A return value "ret" < 0 is an system error
	// A return value "ret" > 0 is an error of the driver
	// "ret" = 0 is no error
	if (ret < 0)
	{
		perror("ioctl");
		return 2;
	}
	else
	{
		if (ret > 0)
		{
			printf("CMD_xpci3xxx_ReadAnalogInputValue error: %d\n", ret);
			return 1;
		}
	}

	// Return the channel value
	*value = arg_dw;

	return 0;
}
//--------------------------------------------------------------------------------
int main(int argc, char** argv)
{
	int * fd;
	int ret = 0;
	unsigned long value = 0;
	unsigned int number_of_boards = 0;
	unsigned char b_ChannelNumber = 0;
	unsigned char b_SelectPolarity = ADDIDATA_UNIPOLAR;
	unsigned char b_SelectGain = ADDIDATA_1_GAIN;
	unsigned char b_SingleDiff = ADDIDATA_SINGLE;

	// This is the first function to call in order to get the board handler
	number_of_boards = xpci3xxx_find_cards(&fd);
	printf("Number of boards in the computer: %d \n",number_of_boards);

	printf ("Channel to use (0 - 15 (depends of the board)):\n");
	b_ChannelNumber = get_integer(0,15);

	printf ("Get the polarity to use (depends of the board):\n");
	printf ("     0 : ADDIDATA_BIPOLAR\n");
	printf ("     1 : ADDIDATA_UNIPOLAR\n");
	b_SelectPolarity = get_integer(0,1);

	printf ("Get the gain to use (depends of the board):\n");
	printf ("     0 : ADDIDATA_1_GAIN\n");
	printf ("     1 : ADDIDATA_2_GAIN\n");
	printf ("     2 : ADDIDATA_5_GAIN\n");
	printf ("     3 : ADDIDATA_10_GAIN\n");
	b_SelectGain = get_integer(0,3);

	printf ("Get the mode to use (depends of the board):\n");
	printf ("     1 : ADDIDATA_DIFFERENTIAL\n");
	printf ("     0 : ADDIDATA_SINGLE\n");
	b_SingleDiff = get_integer(0,1);

	// Read 1 channel value on the first board (fd[0] is the first board, if more board are in the computer the second is fd[1])
	ret = read_1_analog_input (fd[0],				// The board handle that point on /dev/xpci3xxx/0
							   b_ChannelNumber,		// The first channel of the board
							   b_SingleDiff,		// Mode in single ended (can be 1 for differential)
							   b_SelectGain,		// Gain of 1
							   b_SelectPolarity,	// The polarity is in unipolar (0 to 10V, can be bipolar)
							   10,					// The conversion time use by the board to convert the volt in digital value (here 10 us)
							   &value);				// The read value

	// If no error display the value
	if (ret == 0)
		printf ("Value of the first channel = %lu\n", value);


	return 0;
}
//--------------------------------------------------------------------------------
