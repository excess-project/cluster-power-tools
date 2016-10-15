/** @file sample_counter.c
*
* @author S. Weber
*
* @date 24.04.06
* 
* @version  $LastChangedRevision$
* @version $LastChangedDate$
* 
* This software 
* - scans the /proc/sys/xpci3xxx/ directory 
* - run a serie of counter test on each detected boards
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
#include <unistd.h>
// Ucomment when needed
// #include <linux/delay.h>
// #include <errno.h>
// #include <assert.h>

/* Shared memory */
#include <linux/mman.h>
// #include <linux/vmalloc.h>
// #include <linux/wrapper.h>
// #include <linux/signal.h>

#include <xpci3xxx.h>

#include "findboards.h"

int            failed_tests         = 0;
int            global_fd            = 0;
int            i_CurrentBoard       = 0;
unsigned int   xpci3xxx_card_number = 0;

unsigned long ul_CounterInterrupt [3] = {0,0,0};
void v_ControlExit(int i);
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
//	printf("\n\n=> %s\n",__FUNCTION__);
//	printf("\n=> CMD_xpci3xxx_TestInterrupt\n");
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
			/* Test if counter 0 interrupt*/
			if (dw_ArgTable[0] == 0x100) 
				{
                    ul_CounterInterrupt [0] ++;
				}

               /* Test if counter 1 interrupt*/
               if (dw_ArgTable[0] == 0x200) 
                    {
                    ul_CounterInterrupt [1] ++;
                    }

               /* Test if counter 2 interrupt*/
               if (dw_ArgTable[0] == 0x400)
                    {
                    ul_CounterInterrupt [2] ++;
                    }
               } // end of if      (0xfffffffe != dw_ArgTable[0])
  		else
  			{
	   		/* Test if FIFO Full */
	   		if 	((0xffffffff != dw_ArgTable[0]) && (0xfffffffe != dw_ArgTable[0]))
	   			{
		   		printf("\ndw_InterruptSource : %x\n",  dw_ArgTable[0] );
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

	uint32_t dw_Counter      = 0;
	int    i_ioctlReturn  = 0;
     uint8_t   b_NewStatus    = 0;
     uint8_t   b_OldStatus    = 0;

	/* request asynchronous SIGIO */
	{
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
		{
			int oflags = fcntl(fd, F_GETFL);
			if ( fcntl(fd, F_SETFL, oflags | FASYNC) == -1)
			{
				perror("fcntl");
				FAILED();							
			}
		}
	}

	global_fd = fd;



// COUNTER 0 AND 1 AND 2 ARE OK

	printf("\n=> CMD_xpci3xxx_InitCounter - Counter 0\n");
	{
	        uint32_t dw_ArgTable[4];

	        dw_ArgTable[0] = 0;                 //	b_CounterNumber = 0
	        dw_ArgTable[1] = ADDIDATA_DOWN;     //	Count down
	        dw_ArgTable[2] = ADDIDATA_HIGH;     //	High level count
	        dw_ArgTable[3] = 10;                //	Reload value = 10

		i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_InitCounter,dw_ArgTable);
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
		printf("\n=> CMD_xpci3xxx_EnableDisableCounterHardwareOutput - Counter 0\n");
		{
		        uint8_t b_ArgTable[3];
	
		        b_ArgTable[0] = 0; //	b_CounterNumber = 0
		        b_ArgTable[1] = ADDIDATA_ENABLE; //	Enable the hardware Counter output
		        b_ArgTable[2] = ADDIDATA_HIGH; //	If the Counter overflow the output is set to high
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_EnableDisableCounterHardwareOutput,b_ArgTable);
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
	
		printf("\n=> CMD_xpci3xxx_EnableDisableCounterInterrupt - Counter 0 - ENABLE\n");
		{
		        uint8_t b_ArgTable[2];
	
		        b_ArgTable[0] = 0; //	b_CounterNumber = 0
		        b_ArgTable[1] = ADDIDATA_ENABLE; //	enable the interrupt
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_EnableDisableCounterInterrupt,b_ArgTable);
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
	
		printf("\n=> CMD_xpci3xxx_StartCounter - Counter 0\n");
		{
		        uint8_t b_Arg;
	
		        b_Arg = 0; //	b_CounterNumber = 0
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_StartCounter,&b_Arg);
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
	
		printf("\n=> CMD_xpci3xxx_ReadCounterValue - Counter 0\n");
		printf("\n=> CMD_xpci3xxx_ReadCounterStatus - Counter 0\n\n");
		printf("\n=> CMD_xpci3xxx_GetCounterHardwareOutputStatus - Counter 0\n\n");
		
		{
		        uint32_t dw_ArgTable[2];
		        uint8_t b_ArgTable[4];
		        uint8_t b_HardwareOutputStatusArgTable[2];
		
		        dw_ArgTable[0] = 0; //	b_CounterNumber = 0
		        dw_ArgTable[1] = 0; //	dw_CounterValue
                  b_OldStatus = 0;
		
		        b_ArgTable[0] = 0; //	b_CounterNumber = 0
		        
		        b_HardwareOutputStatusArgTable[0] = 0; //	b_CounterNumber = 0
	
			for (dw_Counter = 0 ; dw_Counter < 100000UL ; dw_Counter ++)
			{
				i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReadCounterValue,dw_ArgTable);
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
	
				i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReadCounterStatus,b_ArgTable);
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
	
				i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_GetCounterHardwareOutputStatus,b_HardwareOutputStatusArgTable);
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
	
                    b_NewStatus = (b_ArgTable[1] << 0) | (b_ArgTable[2] << 1) |  (b_ArgTable[3] << 2) | (b_HardwareOutputStatusArgTable[1] << 3);
                    if (b_NewStatus != b_OldStatus)
                       {
                       printf("\n");
                       }
                    b_OldStatus = b_NewStatus;
                    printf("\r\tCounterValue : %5u, CounterStatus: %hx, SoftwareTriggerStatus: %hx, HardwareTriggerStatus: %hx, HardwareOutputStatus: %hx Interrupts: %lu    ",
                            dw_ArgTable[1], b_ArgTable[1],  b_ArgTable[2],  b_ArgTable[3], b_HardwareOutputStatusArgTable[1], ul_CounterInterrupt[0]);
			}
		}
	
		printf("\n\n=> CMD_xpci3xxx_StopCounter - Counter 0\n");
		{
		        uint8_t b_Arg;
	
		        b_Arg = 0; //	b_CounterNumber = 0
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_StopCounter,&b_Arg);
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
	
		printf("\n=> CMD_xpci3xxx_EnableDisableCounterInterrupt - Counter 0 - DISABLE\n");
		{
		        uint8_t b_ArgTable[2];
	
		        b_ArgTable[0] = 0; //	b_CounterNumber = 0
		        b_ArgTable[1] = ADDIDATA_DISABLE; //	disable the interrupt
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_EnableDisableCounterInterrupt,b_ArgTable);
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
	
		printf("\n=> CMD_xpci3xxx_ReleaseCounter - Counter 0\n");
		{
		        uint8_t b_Arg;
	
		        b_Arg = 0; //	b_CounterNumber = 0
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReleaseCounter,&b_Arg);
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

	printf("\n=> CMD_xpci3xxx_InitCounter - Counter 1\n");
	{
	        uint32_t dw_ArgTable[4];

             dw_ArgTable[0] = 1;                 //    b_CounterNumber = 1
             dw_ArgTable[1] = ADDIDATA_DOWN;     //    Count down
             dw_ArgTable[2] = ADDIDATA_HIGH;     //    High level count
             dw_ArgTable[3] = 10;                //    Reload value = 10

		i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_InitCounter,dw_ArgTable);
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
		printf("\n=> CMD_xpci3xxx_EnableDisableCounterHardwareOutput - Counter 1\n");
		{
		        uint8_t b_ArgTable[3];
	
		        b_ArgTable[0] = 1; //	b_CounterNumber = 1
		        b_ArgTable[1] = ADDIDATA_ENABLE; //	Enable the hardware Counter output
		        b_ArgTable[2] = ADDIDATA_HIGH; //	If the Counter overflow the output is set to high
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_EnableDisableCounterHardwareOutput,b_ArgTable);
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
	
		printf("\n=> CMD_xpci3xxx_EnableDisableCounterInterrupt - Counter 1 - ENABLE\n");
		{
		        uint8_t b_ArgTable[2];
	
		        b_ArgTable[0] = 1; //	b_CounterNumber = 1
		        b_ArgTable[1] = ADDIDATA_ENABLE; //	enable the interrupt
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_EnableDisableCounterInterrupt,b_ArgTable);
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
	
		printf("\n=> CMD_xpci3xxx_StartCounter - Counter 1\n");
		{
		        uint8_t b_Arg;
	
		        b_Arg = 1; //	b_CounterNumber = 1
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_StartCounter,&b_Arg);
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
          
          usleep (3000000);
	
		printf("\n=> CMD_xpci3xxx_ReadCounterValue - Counter 1\n");
		printf("\n=> CMD_xpci3xxx_ReadCounterStatus - Counter 1\n\n");
		printf("\n=> CMD_xpci3xxx_GetCounterHardwareOutputStatus - Counter 1\n\n");
		
		{
		        uint32_t dw_ArgTable[2];
		        uint8_t b_ArgTable[4];
		        uint8_t b_HardwareOutputStatusArgTable[2];
		
		        dw_ArgTable[0] = 1; //	b_CounterNumber = 1
		        dw_ArgTable[1] = 0; //	dw_CounterValue
                  b_OldStatus = 0;
		
		        b_ArgTable[0] = 1; //	b_CounterNumber = 1
		        
		        b_HardwareOutputStatusArgTable[0] = 1; //	b_CounterNumber = 1
	
			for (dw_Counter = 0 ; dw_Counter < 100000UL ; dw_Counter ++)
			{
				i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReadCounterValue,dw_ArgTable);
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
	
				i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReadCounterStatus,b_ArgTable);
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
	
				i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_GetCounterHardwareOutputStatus,b_HardwareOutputStatusArgTable);
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
	
                    b_NewStatus = (b_ArgTable[1] << 0) | (b_ArgTable[2] << 1) |  (b_ArgTable[3] << 2) | (b_HardwareOutputStatusArgTable[1] << 3);
                    if (b_NewStatus != b_OldStatus)
                       {
                       printf("\n");
                       }
                    b_OldStatus = b_NewStatus;
                    printf("\r\tCounterValue : %5u, CounterStatus: %hx, SoftwareTriggerStatus: %hx, HardwareTriggerStatus: %hx, HardwareOutputStatus: %hx Interrupts: %lu    ",
                            dw_ArgTable[1], b_ArgTable[1],  b_ArgTable[2],  b_ArgTable[3], b_HardwareOutputStatusArgTable[1], ul_CounterInterrupt[1]);
			}
		}
	
		printf("\n\n=> CMD_xpci3xxx_StopCounter - Counter 1\n");
		{
		        uint8_t b_Arg;
	
		        b_Arg = 1; //	b_CounterNumber = 1
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_StopCounter,&b_Arg);
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
	
		printf("\n=> CMD_xpci3xxx_EnableDisableCounterInterrupt - Counter 1 - DISABLE\n");
		{
		        uint8_t b_ArgTable[2];
	
		        b_ArgTable[0] = 1; //	b_CounterNumber = 1
		        b_ArgTable[1] = ADDIDATA_DISABLE; //	disable the interrupt
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_EnableDisableCounterInterrupt,b_ArgTable);
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
	
		printf("\n=> CMD_xpci3xxx_ReleaseCounter - Counter 1\n");
		{
		        uint8_t b_Arg;
	
		        b_Arg = 1; //	b_CounterNumber = 1
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReleaseCounter,&b_Arg);
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

	printf("\n=> CMD_xpci3xxx_InitCounter - Counter 2\n");
	{
	        uint32_t dw_ArgTable[4];

             dw_ArgTable[0] = 2;                 //    b_CounterNumber = 2
             dw_ArgTable[1] = ADDIDATA_DOWN;     //    Count down
             dw_ArgTable[2] = ADDIDATA_HIGH;     //    High level count
             dw_ArgTable[3] = 10;                //    Reload value = 10

		i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_InitCounter,dw_ArgTable);
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
		printf("\n=> CMD_xpci3xxx_EnableDisableCounterHardwareOutput - Counter 2\n");
		{
		        uint8_t b_ArgTable[3];
	
		        b_ArgTable[0] = 2; //	b_CounterNumber = 2
		        b_ArgTable[1] = ADDIDATA_ENABLE; //	Enable the hardware Counter output
		        b_ArgTable[2] = ADDIDATA_HIGH; //	If the Counter overflow the output is set to high
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_EnableDisableCounterHardwareOutput,b_ArgTable);
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
	
		printf("\n=> CMD_xpci3xxx_EnableDisableCounterInterrupt - Counter 2 - ENABLE\n");
		{
		        uint8_t b_ArgTable[2];
	
		        b_ArgTable[0] = 2; //	b_CounterNumber = 2
		        b_ArgTable[1] = ADDIDATA_ENABLE; //	enable the interrupt
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_EnableDisableCounterInterrupt,b_ArgTable);
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
	
		printf("\n=> CMD_xpci3xxx_StartCounter - Counter 2\n");
		{
		        uint8_t b_Arg;
	
		        b_Arg = 2; //	b_CounterNumber = 2
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_StartCounter,&b_Arg);
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

          usleep (3000000);

		printf("\n=> CMD_xpci3xxx_ReadCounterValue - Counter 2\n");
		printf("\n=> CMD_xpci3xxx_ReadCounterStatus - Counter 2\n\n");
		printf("\n=> CMD_xpci3xxx_GetCounterHardwareOutputStatus - Counter 2\n\n");
		
		{
		        uint32_t dw_ArgTable[2];
		        uint8_t b_ArgTable[4];
		        uint8_t b_HardwareOutputStatusArgTable[2];
		
		        dw_ArgTable[0] = 2; //	b_CounterNumber = 2
		        dw_ArgTable[1] = 0; //	dw_CounterValue
                  b_OldStatus = 0;
		
		        b_ArgTable[0] = 2; //	b_CounterNumber = 2
		        
		        b_HardwareOutputStatusArgTable[0] = 2; //	b_CounterNumber = 2
	
			for (dw_Counter = 0 ; dw_Counter < 100000UL ; dw_Counter ++)
			{
				i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReadCounterValue,dw_ArgTable);
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
	
				i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReadCounterStatus,b_ArgTable);
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
	
				i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_GetCounterHardwareOutputStatus,b_HardwareOutputStatusArgTable);
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
	
                    b_NewStatus = (b_ArgTable[1] << 0) | (b_ArgTable[2] << 1) |  (b_ArgTable[3] << 2) | (b_HardwareOutputStatusArgTable[1] << 3);
                    if (b_NewStatus != b_OldStatus)
                       {
                       printf("\n");
                       }
                    b_OldStatus = b_NewStatus;
                    printf("\r\tCounterValue : %5u, CounterStatus: %hx, SoftwareTriggerStatus: %hx, HardwareTriggerStatus: %hx, HardwareOutputStatus: %hx Interrupts: %lu    ",
                            dw_ArgTable[1], b_ArgTable[1],  b_ArgTable[2],  b_ArgTable[3], b_HardwareOutputStatusArgTable[1], ul_CounterInterrupt[2]);
			}
		}
	
		printf("\n\n=> CMD_xpci3xxx_StopCounter - Counter 2\n");
		{
		        uint8_t b_Arg;
	
		        b_Arg = 2; //	b_CounterNumber = 2
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_StopCounter,&b_Arg);
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
	
		printf("\n=> CMD_xpci3xxx_EnableDisableCounterInterrupt - Counter 2 - DISABLE\n");
		{
		        uint8_t b_ArgTable[2];
	
		        b_ArgTable[0] = 2; //	b_CounterNumber = 2
		        b_ArgTable[1] = ADDIDATA_DISABLE; //	disable the interrupt
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_EnableDisableCounterInterrupt,b_ArgTable);
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
	
		printf("\n=> CMD_xpci3xxx_ReleaseCounter - Counter 2\n");
		{
		        uint8_t b_Arg;
	
		        b_Arg = 2; //	b_CounterNumber = 2
	
			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReleaseCounter,&b_Arg);
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
//--------------------------------------------------------------------------------
int main(int argc, char** argv)
{

	int * xpci3xxx_card_fd;
	{
		xpci3xxx_card_number = xpci3xxx_find_cards(&xpci3xxx_card_fd);
		
		printf("total: %d cards\n",xpci3xxx_card_number);
	}	
	

     if (xpci3xxx_card_number)
        {
        signal(SIGINT,v_ControlExit);
        }

     {
		for (i_CurrentBoard=0;i_CurrentBoard< xpci3xxx_card_number; i_CurrentBoard++)
		{
			printf("\n++++++++++++++++++++++++++++++++++++++++\nrunning test on device of minor number %d\n++++++++++++++++++++++++++++++++++++++++\n",i_CurrentBoard);
			xpci3xxx_run_test(xpci3xxx_card_fd[i_CurrentBoard]);
		}
	}

	

	return 0;
}
//--------------------------------------------------------------------------------

void v_ControlExit(int i)
{
   uint8_t b_Counter;
   uint8_t b_ArgTable[2];

   if ((xpci3xxx_card_number) && (i_CurrentBoard < xpci3xxx_card_number))
      {
      // Stop all counters

      b_Counter = 0;
      ioctl (global_fd,CMD_xpci3xxx_StopCounter,&b_Counter);
      b_Counter = 1;
      ioctl (global_fd,CMD_xpci3xxx_StopCounter,&b_Counter);
      b_Counter = 2;
      ioctl (global_fd,CMD_xpci3xxx_StopCounter,&b_Counter);

      // Disable all counters interrupts

      b_ArgTable[1] = ADDIDATA_DISABLE; // disable the interrupt
      b_ArgTable[0] = 0; // Counter 0
      ioctl (global_fd,CMD_xpci3xxx_EnableDisableCounterInterrupt,b_ArgTable);
      b_ArgTable[0] = 1; // Counter 0
      ioctl (global_fd,CMD_xpci3xxx_EnableDisableCounterInterrupt,b_ArgTable);
      b_ArgTable[0] = 2; // Counter 0
      ioctl (global_fd,CMD_xpci3xxx_EnableDisableCounterInterrupt,b_ArgTable);

      // Release all counters

      b_Counter = 0;
      ioctl (global_fd,CMD_xpci3xxx_ReleaseCounter,&b_Counter);
      b_Counter = 1;
      ioctl (global_fd,CMD_xpci3xxx_ReleaseCounter,&b_Counter);
      b_Counter = 2;
      ioctl (global_fd,CMD_xpci3xxx_ReleaseCounter,&b_Counter);

      close (global_fd);
      }

    printf ("\n+---------------------------------------------------+\n");
    printf ("| Program concluded.                                |\n");
    printf ("+---------------------------------------------------+\n");
    exit(0);
}

//--------------------------------------------------------------------------------
