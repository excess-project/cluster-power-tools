/** @file testxpci3xxx.c
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
// Ucomment when needed
// #include <linux/delay.h>
// #include <errno.h>
// #include <assert.h>

/* Shared memory */
#include <sys/mman.h>

#include <xpci3xxx.h>

#include "findboards.h"

/*Number of iterations*/
/*for a MSX-Box*/
#define NB_LOOP 100
#define NB_LOOP_TIMER 300
/*for a x86*/
//#define NB_LOOP 100000
//#define NB_LOOP_TIMER 30000


int 	failed_tests = 0;
int 	global_fd = 0;
uint16_t* 	pw_Value = NULL;	

uint8_t b_NumberOfChannels = 0;
	
//------------------------------------------------------------------------------
void FAILED(void) 
{
	printf("failed\n");
	failed_tests++;
}

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
void xpci3xxx_signal(int a)
{

	int i_ioctlReturn = 0;
	printf("\n\n=> %s\n",__FUNCTION__);
	printf("\n=> CMD_xpci3xxx_TestInterrupt\n");
	{
	
	uint32_t dw_ArgTable[1 + ADDIDATA_MAX_AI + 1 + ADDIDATA_MAX_INTERRUPT_ANALOG_INPUT_VALUE];

	memset(dw_ArgTable, 0 , sizeof (dw_ArgTable));

	while (0xfffffffe != dw_ArgTable[0])
		{
		i_ioctlReturn = ioctl (global_fd,CMD_xpci3xxx_TestInterrupt, dw_ArgTable);
			if (i_ioctlReturn < 0)
			{
				perror("ioctl");
				exit(1);
			}			
			else 
			{
				if ( i_ioctlReturn > 0 )
				{
					printf("An error occur. Code: %d\n", i_ioctlReturn);
					exit(1);
				}
			}			
   		/* Test if FIFO Empty */
   		if 	(0xfffffffe != dw_ArgTable[0])
   			{
	   		printf("\ndw_InterruptSource : %x",  dw_ArgTable[0] ); 
			
  			} // end of if 	(0xfffffffe != dw_ArgTable[0])
  		else
  			{
	   		/* Test if FIFO Full */
	   		if 	(0xffffffff != dw_ArgTable[0])
	   			{
		   		printf("\ndw_InterruptSource : %x",  dw_ArgTable[0] ); 
		   		break;
	  			}
  			}
		} // end of while (0xfffffffe != dw_ArgTable[0])
	}
	//gettimeofday(&end, NULL);
}

//--------------------------------------------------------------------------------
void xpci3xxx_run_test(int fd)
{

	uint32_t dw_Counter = 0;
	int i_ioctlReturn = 0;
	

	printf("\n=> CMD_xpci3xxx_GetHardwareInformation\n\n");
	{
		
		struct xpci3xxx_USER_str_BoardInformations xpci3xxx_USER_s_BoardInformations;
		i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_GetHardwareInformation,&xpci3xxx_USER_s_BoardInformations);
			if (i_ioctlReturn < 0)
			{
				perror("ioctl");
				exit(1);
			}			
			else 
			{
				if ( i_ioctlReturn > 0 )
				{
					printf("An error occur. Code: %d\n", i_ioctlReturn);
					exit(1);
				}
			}			

   		printf("\tBAR 0 : %x\n",  xpci3xxx_USER_s_BoardInformations.dw_BoardBaseAddress[0] ); 
		printf("\tBAR 1 : %x\n",  xpci3xxx_USER_s_BoardInformations.dw_BoardBaseAddress[1] ); 
		printf("\tBAR 2 : %x\n",  xpci3xxx_USER_s_BoardInformations.dw_BoardBaseAddress[2] ); 
		printf("\tBAR 3 : %x\n",  xpci3xxx_USER_s_BoardInformations.dw_BoardBaseAddress[3] ); 
   		printf("\tinterrupt nb: %hu\n", xpci3xxx_USER_s_BoardInformations.b_InterruptNbr);
   		printf("\tslot number: %hu\n", xpci3xxx_USER_s_BoardInformations.b_SlotNumber);
	}

	/* request asynchronous SIGIO */
	{
	int oflags = fcntl(fd, F_GETFL);
		printf("\n=> Requesting SIGIO notification\n");
		if (signal(SIGIO,&xpci3xxx_signal) == SIG_ERR)
		{
			perror("signal");
			FAILED();
		}
		if (fcntl(fd, F_SETOWN, getpid()) == -1)
		{
			perror("fcntl");
			FAILED();				
		}
		
		if ( fcntl(fd, F_SETFL, oflags | FASYNC) == -1)
		{
			perror("fcntl");
			FAILED();							
		}
	}

	global_fd = fd;

	printf("\n=> CMD_xpci3xxx_Set32DigitalOutputsOn\n");
	printf("\n=> CMD_xpci3xxx_Get32DigitalOutputStatus\n");
	{
		
		uint32_t dw_Arg = 0;
		for (dw_Counter = 0 ; dw_Counter < 2 ; dw_Counter ++)
		{
			dw_Arg = 0x1 << (dw_Counter % 2);
			
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_Set32DigitalOutputsOn,&dw_Arg);
				if (i_ioctlReturn < 0)
				{
					perror("ioctl");
					exit(1);
				}			
				else 
				{
					if ( i_ioctlReturn > 0 )
					{
						printf("An error occur. Code: %d\n", i_ioctlReturn);
						exit(1);
					}
				}			

	   		printf("\n\tValue written: %x,", dw_Arg ); 
	   		usleep (500000);

			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_Get32DigitalOutputStatus,&dw_Arg);
				if (i_ioctlReturn < 0)
				{
					perror("ioctl");
					exit(1);
				}			
				else 
				{
					if ( i_ioctlReturn > 0 )
					{
						printf("An error occur. Code: %d\n", i_ioctlReturn);
						exit(1);
					}
				}			

	   		printf("value read : %x", dw_Arg ); 
	   		usleep (500000);
	   		
		}

		dw_Arg = 0x0;
		i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_Set32DigitalOutputsOn,&dw_Arg);
			if (i_ioctlReturn < 0)
			{
				perror("ioctl");
				exit(1);
			}			
			else 
			{
				if ( i_ioctlReturn > 0 )
				{
					printf("An error occur. Code: %d\n", i_ioctlReturn);
					exit(1);
				}
			}			

		printf("\n\tValue written: %x,", dw_Arg ); 
		usleep (500000);

		i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_Get32DigitalOutputStatus,&dw_Arg);
			if (i_ioctlReturn < 0)
			{
				perror("ioctl");
				exit(1);
			}			
			else 
			{
				if ( i_ioctlReturn > 0 )
				{
					printf("An error occur. Code: %d\n", i_ioctlReturn);
					exit(1);
				}
			}			

	   	printf("value read : %x", dw_Arg ); 
	   	usleep (500000);
	}

// TIMER 0

	printf("\n=> CMD_xpci3xxx_InitTimer - Timer 0\n");
	{
		
	        uint32_t dw_ArgTable[4];

	        dw_ArgTable[0] = 0; //	b_TimerNumber = 0
	        dw_ArgTable[1] = 2; //	b_TimerMode = 2
	        dw_ArgTable[2] = 2; //	b_TimerTimeUnit = micro s
	        dw_ArgTable[3] = 4000; //	dw_ReloadValue = 4000

		i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_InitTimer,dw_ArgTable);
			if (i_ioctlReturn < 0)
			{
				perror("ioctl");
				exit(1);
			}			
			else 
			{
				if ( i_ioctlReturn > 0 )
				{
				printf("An error occur. Code: %d", i_ioctlReturn);
				if ( i_ioctlReturn != 2 )
					{
					printf("\n");
					exit(1);
					}
				else
					{
					printf(" - functionality not available\n");
					}
				}
			}			
	}

	if ( i_ioctlReturn != 2 )
		{
		printf("\n=> CMD_xpci3xxx_EnableDisableTimerHardwareOutput - Timer 0\n");
		{
			
		        uint8_t b_ArgTable[3];
	
		        b_ArgTable[0] = 0; //	b_TimerNumber = 0
		        b_ArgTable[1] = ADDIDATA_ENABLE; //	Enable the hardware timer output
		        b_ArgTable[2] = ADDIDATA_HIGH; //	If the timer overflow the output is set to high
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_EnableDisableTimerHardwareOutput,b_ArgTable);
				if (i_ioctlReturn < 0)
				{
					perror("ioctl");
					exit(1);
				}			
				else 
				{
					if ( i_ioctlReturn > 0 )
					{
						printf("An error occur. Code: %d\n", i_ioctlReturn);
						exit(1);
					}
				}			
		}
	
		printf("\n=> CMD_xpci3xxx_EnableDisableTimerInterrupt - Timer 0 - ENABLE\n");
		{
			
		        uint8_t b_ArgTable[2];
	
		        b_ArgTable[0] = 0; //	b_TimerNumber = 0
		        b_ArgTable[1] = ADDIDATA_ENABLE; //	enable the interrupt
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_EnableDisableTimerInterrupt,b_ArgTable);
				if (i_ioctlReturn < 0)
				{
					perror("ioctl");
					exit(1);
				}			
				else 
				{
					if ( i_ioctlReturn > 0 )
					{
						printf("An error occur. Code: %d\n", i_ioctlReturn);
						exit(1);
					}
				}			
		}
	
		printf("\n=> CMD_xpci3xxx_StartTimer - Timer 0\n");
		{
			
		        uint8_t b_Arg;
	
		        b_Arg = 0; //	b_TimerNumber = 0
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_StartTimer,&b_Arg);
				if (i_ioctlReturn < 0)
				{
					perror("ioctl");
					exit(1);
				}			
				else 
				{
					if ( i_ioctlReturn > 0 )
					{
						printf("An error occur. Code: %d\n", i_ioctlReturn);
						exit(1);
					}
				}			
		}
	
		printf("\n=> CMD_xpci3xxx_ReadTimerValue - Timer 0\n");
		printf("\n=> CMD_xpci3xxx_ReadTimerStatus - Timer 0\n\n");
		printf("\n=> CMD_xpci3xxx_GetTimerHardwareOutputStatus - Timer 0\n\n");
		
		{
			
		        uint32_t dw_ArgTable[2];
		        uint8_t b_ArgTable[4];
		        uint8_t b_HardwareOutputStatusArgTable[2];
		
		        dw_ArgTable[0] = 0; //	b_TimerNumber = 0
		        dw_ArgTable[1] = 0; //	dw_TimerValue
		
		        b_ArgTable[0] = 0; //	b_TimerNumber = 0
		        
		        b_HardwareOutputStatusArgTable[0] = 0; //	b_TimerNumber = 0
	
			for (dw_Counter = 0 ; dw_Counter < NB_LOOP_TIMER ; dw_Counter ++)
			{
				i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReadTimerValue,dw_ArgTable);
					if (i_ioctlReturn < 0)
					{
						perror("ioctl");
						exit(1);
					}			
					else 
					{
						if ( i_ioctlReturn > 0 )
						{
							printf("An error occur. Code: %d\n", i_ioctlReturn);
							exit(1);
						}
					}			
		   		printf("\r\tTimerValue : %x,",  dw_ArgTable[1] ); 
	
				i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReadTimerStatus,b_ArgTable);
					if (i_ioctlReturn < 0)
					{
						perror("ioctl");
						exit(1);
					}			
					else 
					{
						if ( i_ioctlReturn > 0 )
						{
							printf("An error occur. Code: %d\n", i_ioctlReturn);
							exit(1);
						}
					}			
	
				i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_GetTimerHardwareOutputStatus,b_HardwareOutputStatusArgTable);
					if (i_ioctlReturn < 0)
					{
						perror("ioctl");
						exit(1);
					}			
					else 
					{
						if ( i_ioctlReturn > 0 )
						{
							printf("An error occur. Code: %d\n", i_ioctlReturn);
							exit(1);
						}
					}			
	
		   		printf("TimerStatus: %hx, SoftwareTriggerStatus: %hx, HardwareTriggerStatus: %hx, HardwareOutputStatus: %hx",  b_ArgTable[1],  b_ArgTable[2],  b_ArgTable[3], b_HardwareOutputStatusArgTable[1]); 
		   		if ((b_ArgTable[1] != 0) || (b_ArgTable[2] != 0) || (b_ArgTable[3] != 0) || (b_HardwareOutputStatusArgTable[1] != 0))
		   			printf("\n\n");
			}
		}
	
		printf("\n\n=> CMD_xpci3xxx_StopTimer - Timer 0\n");
		{
			
		        uint8_t b_Arg;
	
		        b_Arg = 0; //	b_TimerNumber = 0
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_StopTimer,&b_Arg);
				if (i_ioctlReturn < 0)
				{
					perror("ioctl");
					exit(1);
				}			
				else 
				{
					if ( i_ioctlReturn > 0 )
					{
						printf("An error occur. Code: %d\n", i_ioctlReturn);
						exit(1);
					}
				}			
		}
	
		printf("\n=> CMD_xpci3xxx_EnableDisableTimerInterrupt - Timer 0 - DISABLE\n");
		{
			
		        uint8_t b_ArgTable[2];
	
		        b_ArgTable[0] = 0; //	b_TimerNumber = 0
		        b_ArgTable[1] = ADDIDATA_DISABLE; //	disable the interrupt
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_EnableDisableTimerInterrupt,b_ArgTable);
				if (i_ioctlReturn < 0)
				{
					perror("ioctl");
					exit(1);
				}			
				else 
				{
					if ( i_ioctlReturn > 0 )
					{
						printf("An error occur. Code: %d\n", i_ioctlReturn);
						exit(1);
					}
				}			
		}
	
		printf("\n=> CMD_xpci3xxx_ReleaseTimer - Timer 0\n");
		{
			
		        uint8_t b_Arg;
	
		        b_Arg = 0; //	b_TimerNumber = 0
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReleaseTimer,&b_Arg);
				if (i_ioctlReturn < 0)
				{
					perror("ioctl");
					exit(1);
				}			
				else 
				{
					if ( i_ioctlReturn > 0 )
					{
						printf("An error occur. Code: %d\n", i_ioctlReturn);
						exit(1);
					}
				}			
		}

	}

	printf("\n=> CMD_xpci3xxx_InitAnalogOutput - Channel 0\n");
	{
		uint8_t b_ArgTable[2];
		b_ArgTable[0] = 0; // Channel 
		b_ArgTable[1] = ADDIDATA_BIPOLAR; // Bipolar

		i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_InitAnalogOutput,b_ArgTable);
			if (i_ioctlReturn < 0)
			{
				perror("ioctl");
				exit(1);
			}			
			else 
			{
				if ( i_ioctlReturn > 0 )
				{
				printf("An error occur. Code: %d", i_ioctlReturn);
				if ( i_ioctlReturn != 2 )
					{
					printf("\n");
					exit(1);
					}
				else
					{
					printf(" - functionality not available\n");
					}
				}
			}			
	}

	if ( i_ioctlReturn != 2 )
		{

		printf("\n=> CMD_xpci3xxx_ReadAnalogOutputBit\n");
		printf("\n=> CMD_xpci3xxx_WriteAnalogOutputValue - Channel 0\n");
		{
			uint32_t dw_MaxLoop = 0;
	
			uint8_t   b_AnalogOutputConversionFinished = 0;
			uint32_t dw_ArgTable[2];
	
			uint8_t   b_Channel = 0;
			uint32_t dw_WriteValue = 16382; // 10 V for Bipolar
			
			dw_ArgTable[0] = b_Channel;
			dw_ArgTable[1] = dw_WriteValue;
	
			while((!b_AnalogOutputConversionFinished) && (dw_MaxLoop < NB_LOOP))
				{
				i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReadAnalogOutputBit,&b_AnalogOutputConversionFinished);
					if (i_ioctlReturn < 0)
					{
						perror("ioctl");
						exit(1);
					}			
					else 
					{
						if ( i_ioctlReturn > 0 )
						{
							printf("An error occur. Code: %d\n", i_ioctlReturn);
							exit(1);
						}
					}			
				}			
	
			if (b_AnalogOutputConversionFinished)
				{
				i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_WriteAnalogOutputValue,dw_ArgTable);
					if (i_ioctlReturn < 0)
					{
						perror("ioctl");
						exit(1);
					}			
					else 
					{
						if ( i_ioctlReturn > 0 )
						{
							printf("An error occur. Code: %d\n", i_ioctlReturn);
							exit(1);
						}
					}			
				}
		}
	
		printf("\n=> CMD_xpci3xxx_InitAnalogOutput - Channel 3\n");
		{
			uint8_t b_ArgTable[2];
			b_ArgTable[0] = 3; // Channel
			b_ArgTable[1] = ADDIDATA_UNIPOLAR; // Unipolar
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_InitAnalogOutput,b_ArgTable);
				if (i_ioctlReturn < 0)
				{
					perror("ioctl");
					exit(1);
				}			
				else 
				{
					if ( i_ioctlReturn > 0 )
					{
						printf("An error occur. Code: %d\n", i_ioctlReturn);
						exit(1);
					}
				}			
	
		}
	
		printf("\n=> CMD_xpci3xxx_ReadAnalogOutputBit\n");
		printf("\n=> CMD_xpci3xxx_WriteAnalogOutputValue - Channel 3\n");
		{
			
			uint32_t dw_MaxLoop = 0;
	
			uint8_t   b_AnalogOutputConversionFinished = 0;
			uint32_t dw_ArgTable[2];
	
			uint8_t   b_Channel = 3;
			uint32_t dw_WriteValue = 4916; // 6 V for Bipolar
			
			dw_ArgTable[0] = b_Channel;
			dw_ArgTable[1] = dw_WriteValue;
	
			while((!b_AnalogOutputConversionFinished) && (dw_MaxLoop < NB_LOOP))
				{
				i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReadAnalogOutputBit,&b_AnalogOutputConversionFinished);
					if (i_ioctlReturn < 0)
					{
						perror("ioctl");
						exit(1);
					}			
					else 
					{
						if ( i_ioctlReturn > 0 )
						{
							printf("An error occur. Code: %d\n", i_ioctlReturn);
							exit(1);
						}
					}			
				}			
	
			if (b_AnalogOutputConversionFinished)
				{
				i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_WriteAnalogOutputValue,dw_ArgTable);
					if (i_ioctlReturn < 0)
					{
						perror("ioctl");
						exit(1);
					}			
					else 
					{
						if ( i_ioctlReturn > 0 )
						{
							printf("An error occur. Code: %d\n", i_ioctlReturn);
							exit(1);
						}
					}			
				}
		}
	}
sleep (2);



	printf("\n=> CMD_xpci3xxx_Read32DigitalInputs\n");
	{
		
		uint32_t dw_Arg = 0;

		i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_Read32DigitalInputs,&dw_Arg);
			if (i_ioctlReturn < 0)
			{
				perror("ioctl");
				exit(1);
			}			
			else 
			{
				if ( i_ioctlReturn > 0 )
				{
					printf("An error occur. Code: %d\n", i_ioctlReturn);
					exit(1);
				}
			}			

   		printf("\nValue read : %x\n\n", dw_Arg ); 
	   		
	}

}
//--------------------------------------------------------------------------------
int main(int argc, char** argv)
{

	int * xpci3xxx_card_fd;
	unsigned int xpci3xxx_card_number = 0;
	{
		xpci3xxx_card_number = xpci3xxx_find_cards(&xpci3xxx_card_fd);
		
		printf("total: %d cards\n",xpci3xxx_card_number);
	}	
	
	{
		int i = 0;
		for (i=0;i< xpci3xxx_card_number; i++)
		{
			printf("\n++++++++++++++++++++++++++++++++++++++++\nrunning test on device of minor number %d\n++++++++++++++++++++++++++++++++++++++++\n",i);
			xpci3xxx_run_test(xpci3xxx_card_fd[i]);		
		}
	}

	

	return 0;
}
//--------------------------------------------------------------------------------
