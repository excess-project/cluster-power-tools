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
// #include <assert.h>

#include <xpci3xxx.h>

#include "errorlib.h"
#include "findboards.h"
#include "errormsg.h"
#include "input.h"

/*Number of iterations*/
#define NB_MAX_LOOP 100000

//--------------------------------------------------------------------------------
/** wait for the board to be ready
 * @retval 0 success
 * @retval -1 system error or timeout
 */
int xpci3xxx_wait_for_analog_output(int fd)
{
	unsigned int counter = 0;
	unsigned char b_AnalogOutputConversionFinished = 0;

	/* this is implemented with busy looping and could profitably done with a timer */
	while(!b_AnalogOutputConversionFinished)
	{
		if(counter>NB_MAX_LOOP) {
			printf("%s: timeout",__FUNCTION__);
			return -1;
		}
		if( call_ioctl (fd,CMD_xpci3xxx_ReadAnalogOutputBit,&b_AnalogOutputConversionFinished) )
			return -1;
		counter++;
	}
	return 0;
}
//--------------------------------------------------------------------------------

int xpci3xxx_init_output(int fd, int nb, char channel, char type)
{
	printf("Initializing output of board %d\n",nb);
	/* wait for the board to be ready */
	if( xpci3xxx_wait_for_analog_output(fd) )
		return -1;

	/* Initialization of the output */
	{
		unsigned char b_ArgTable[2];
		b_ArgTable[0] = channel; // Channel
		b_ArgTable[1] = type;    // Polarity

		if( call_ioctl (fd,CMD_xpci3xxx_InitAnalogOutput,b_ArgTable) )
		{
			return 11;
		}
	}
	return 0;
}
//--------------------------------------------------------------------------------
/** write an analog output
 * @retval 0 success
 * @retval -1 system error or timeout
 */
int xpci3xxx_set_output(int fd, int nb, char channel, uint32_t value)
{
	/* wait for the board to be ready */
	if( xpci3xxx_wait_for_analog_output(fd) )
		return -1;

	/* write value */
	{
		uint32_t dw_ArgTable[2];
		dw_ArgTable[0] = channel;
		dw_ArgTable[1] = value;

		return call_ioctl (fd,CMD_xpci3xxx_WriteAnalogOutputValue,dw_ArgTable);
	}
	return 0;
}
//--------------------------------------------------------------------------------
int main (int argc, char** argv)
{

	/* detect available boards */
	int * xpci3xxx_card_fd;
	int xpci3xxx_card_number = 0;
	{
		xpci3xxx_card_number = apci_find_boards("xpci3xxx", &xpci3xxx_card_fd);
		printf("total: %d cards\n",xpci3xxx_card_number);
		if(xpci3xxx_card_number<0)
			exit(1);
	}

	{
		char channel = 0; /* range depends actually of the type of the board */
		char type = 0; /* ADDIDATA_UNIPOLAR, ADDIDATA_BIPOLAR */
		uint32_t value = 0; /* range depends actually of the type of the board */

		/* request user setup */

		printf("channel number ?");
		channel = get_integer(0, 15);

		printf("output type ?\n");
		printf("ADDIDATA_BIPOLAR: 0: -10V - +10V\n");
		printf("ADDIDATA_UNIPOLAR: 1: 0V - +10V\n");
		type = get_integer(0, 1);

		printf("value ?\n");
		printf("for an APCI3501\n");
		printf("\tADDIDATA_UNIPOLAR: (0 ... 2047)\n");
		printf("\tADDIDATA_BIPOLAR: (0 ... 4095)\n");

		printf("for an APCIe3121\n");
		printf("\tADDIDATA_UNIPOLAR: (0 ... 32767)\n");
		printf("\tADDIDATA_BIPOLAR: (0 ... 65535)\n");

		printf("otherwise\n");
		printf("\tADDIDATA_UNIPOLAR: (0 ... 8191)\n");
		printf("\tADDIDATA_BIIPOLAR: (0 ... 16383)\n");
		value = get_integer(0, 65535);

		/* set the output on all board */
		{
			int i = 0;
			for (i=0;i< xpci3xxx_card_number; i++)
			{
				xpci3xxx_init_output(xpci3xxx_card_fd[i], i, channel, type);
				xpci3xxx_set_output(xpci3xxx_card_fd[i], i, channel, value);
			}
		}
	}

	return 0;
}
