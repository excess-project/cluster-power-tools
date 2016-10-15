/** @file sample_xpci3xxx.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
* 
* @version  $LastChangedRevision$
* @version $LastChangedDate$
* 
* This software 
* - scans the /proc/sys/xpci3xxx/ directory 
* - run a serie of test on each detected boards
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

/* Shared memory */
#include <sys/mman.h>

#include <xpci3xxx.h>

#include "errorlib.h"
#include "findboards.h"
#include "errormsg.h"
#include "input.h"

static int _quit_sample_;

//--------------------------------------------------------------------------------

void quit_sample (int dummy)
{
	_quit_sample_ = 1;               
}

//--------------------------------------------------------------------------------
void xpci3xxx_run_test(int fd)
{
	uint16_t    nbrOfLoop = 0;
	uint8_t    b_NumberOfChannels = 0;
		
	{
		uint8_t 	b_ChannelList[16];
		uint8_t 	b_Gain[16];
		uint8_t 	b_Polarity[16];
		uint8_t 	b_SingleDiff = ADDIDATA_DIFFERENTIAL;
		uint32_t  	dw_NumberOfSequence = 0; // Continuous
		uint32_t dw_ArgTable[(3 * ADDIDATA_MAX_AI) + 6];
		uint8_t	b_ChannelCounter = 0;
		uint8_t    b_DelayMode = 0;
		uint8_t    b_DelayTimeUnit = 0;
		uint8_t    b_SelectPolarity = ADDIDATA_UNIPOLAR;
		uint8_t    b_SelectGain = ADDIDATA_1_GAIN;
		uint16_t    w_DelayTime = 0;


		printf ("Number of channels to use:\n");
		b_NumberOfChannels = get_integer(1,16);

		printf ("Get the polarity to use:\n");
		printf ("     0 : ADDIDATA_BIPOLAR\n");
		printf ("     1 : ADDIDATA_UNIPOLAR\n");
		b_SelectPolarity = get_integer(0,1);

		printf ("Get the gain to use:\n");
		printf ("     0 : ADDIDATA_1_GAIN\n");
		printf ("     1 : ADDIDATA_2_GAIN\n");
		printf ("     2 : ADDIDATA_5_GAIN\n");
		printf ("     3 : ADDIDATA_10_GAIN\n");
		b_SelectGain = get_integer(0,3);

		printf ("Get the mode to use:\n");
		printf ("     1 : ADDIDATA_DIFFERENTIAL\n");
		printf ("     0 : ADDIDATA_SINGLE\n");
		b_SingleDiff = get_integer(0,1);

		{
			int i;
			for (i=0; i<b_NumberOfChannels; i++) 
			{
				b_ChannelList[i] = i;
				b_Polarity[i] = b_SelectPolarity;
				b_Gain[i] = b_SelectGain;
			}
		}

		memset(dw_ArgTable,0,sizeof(dw_ArgTable));

		/* Set parameters of the initialization function */
		dw_ArgTable[0] = (uint32_t)b_NumberOfChannels;

		for (b_ChannelCounter = 0 ; b_ChannelCounter < b_NumberOfChannels ; b_ChannelCounter++)
			dw_ArgTable[1 + b_ChannelCounter] = (uint32_t)b_ChannelList[b_ChannelCounter];
	
		for (b_ChannelCounter = b_NumberOfChannels ; b_ChannelCounter < (2 * b_NumberOfChannels) ; b_ChannelCounter++)
			dw_ArgTable[1 + b_ChannelCounter] = (uint32_t)b_Gain[b_ChannelCounter - b_NumberOfChannels];
	
		for (b_ChannelCounter = (2 * b_NumberOfChannels) ; b_ChannelCounter < (3 * b_NumberOfChannels) ; b_ChannelCounter++)
			dw_ArgTable[1 + b_ChannelCounter] = (uint32_t)b_Polarity[b_ChannelCounter - (2 * b_NumberOfChannels)];
	
		dw_ArgTable[(3 * b_NumberOfChannels) + 1] = (uint32_t)b_SingleDiff;
		dw_ArgTable[(3 * b_NumberOfChannels) + 2] = dw_NumberOfSequence;
		dw_ArgTable[(3 * b_NumberOfChannels) + 3] = (uint32_t)b_DelayMode;
		dw_ArgTable[(3 * b_NumberOfChannels) + 4] = (uint32_t)b_DelayTimeUnit;
		dw_ArgTable[(3 * b_NumberOfChannels) + 5] = (uint32_t)w_DelayTime;

		if (call_ioctl(fd,CMD_xpci3xxx_InitAnalogInputAutoRefresh,dw_ArgTable))
			exit(1);	
			
		printf("CMD_xpci3xxx_InitAnalogInputAutoRefresh\n");		
	}
	
	{
		uint16_t   w_ArgTable[2];
		
		printf ("Conversion time unit:\n");
		printf ("     1 : ADDIDATA_MICRO_SECOND\n");
		printf ("     2 : ADDIDATA_MILLI_SECOND\n");
		w_ArgTable[0] = get_integer(1,2);

		printf ("Conversion time value:\n");
		w_ArgTable[1] = get_integer(1,65535);	

		if (call_ioctl (fd,CMD_xpci3xxx_StartAnalogInputAutoRefresh,w_ArgTable))
			goto stop;

		printf("CMD_xpci3xxx_StartAnalogInputAutoRefresh\n");
	}

	{
		uint32_t dw_ArgTable[ADDIDATA_MAX_AI + 1];
		unsigned int dw_Counter;

		printf ("Get the number of autorefresh cycle to do (0: continuous):\n");
		nbrOfLoop = get_integer(0,1000);

		/* print header */
		printf("Counter\t");
		{
			int i;
			for(i=0; i<b_NumberOfChannels; i++)
			{
				printf("CH%i",i);
				if(i!=(b_NumberOfChannels-1))
					printf("\t");
				else
					printf("\n");
			}
		}

		for (dw_Counter = 0 ; (!_quit_sample_); dw_Counter ++)
		{
			memset(dw_ArgTable, 0, sizeof(dw_ArgTable));
			if (call_ioctl (fd,CMD_xpci3xxx_ReadAnalogInputAutoRefreshValueAndCounter,dw_ArgTable))
				goto stop;

			/* counter */
			printf ("%u\t", dw_ArgTable[ADDIDATA_MAX_AI]);
			
			/* data */
			{
				int i;
				for (i=0; (i<b_NumberOfChannels) && (!_quit_sample_); i++)
				{
					printf("%u", dw_ArgTable[i]);
					if(i!=(b_NumberOfChannels-1))
						printf("\t");
					else
						printf("\n");
				}
			}
			usleep(10);

			if ((nbrOfLoop) && (dw_Counter >= nbrOfLoop))
				break;
		}		
	}

	
	call_ioctl (fd,CMD_xpci3xxx_StopAnalogInputAutoRefresh,NULL);
	printf("CMD_xpci3xxx_StopAnalogInputAutoRefresh\n");

stop:
	call_ioctl (fd,CMD_xpci3xxx_ReleaseAnalogInputAutoRefresh,NULL);
}
//--------------------------------------------------------------------------------
int main (int argc, char** argv)
{

	int * xpci3xxx_card_fd;
	int xpci3xxx_card_number = 0;
	{
		xpci3xxx_card_number = apci_find_boards("xpci3xxx", &xpci3xxx_card_fd);
		printf("total: %d cards\n",xpci3xxx_card_number);
		if(xpci3xxx_card_number<0)
			exit(1);
	}	
	
	printf ("Configuration parameters are to be set according to possibilities of the board you are using.\n");
	printf ("After measure start, use Ctrl+C to stop the sample or wait the end\n");
	
	_quit_sample_ = 0;
	signal(SIGINT, quit_sample);
	
	{
		int i = 0;
		for (i=0;i< xpci3xxx_card_number; i++)
		{
			xpci3xxx_run_test(xpci3xxx_card_fd[i]);		
		}
	}

	return 0;
}
//--------------------------------------------------------------------------------
