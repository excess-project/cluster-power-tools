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

				/* Test the interrupt source */
				if (dw_ArgTable[0] == 0x2) // analog input irq without DMA
				{
					uint8_t b_ChannelCounter = 0;

					for (b_ChannelCounter = 0 ; b_ChannelCounter <= b_NumberOfChannels ; b_ChannelCounter++)
					{
						if (b_ChannelCounter == b_NumberOfChannels)
							printf ("Analog Input counter : %u\n", dw_ArgTable[b_ChannelCounter + 1]);
						else
							printf ("Analog Input (initialisation index) %hu value: %u\n", b_ChannelCounter, dw_ArgTable[b_ChannelCounter + 1]);
					}
				}
				if (dw_ArgTable[0] == 0x1) // analog input irq with DMA
				{
					uint8_t 	b_AdresseCounter = 0;

					printf ("Number of analog Input DMA buffer : %u\n", dw_ArgTable[1 + ADDIDATA_MAX_AI + 1]);

					// Test if there is just one buffer
					if (dw_ArgTable[1 + ADDIDATA_MAX_AI + 1] == 1)
					{
						printf ("Analog Input DMA buffer : Size=%u, Adresse=%x\n", dw_ArgTable[1 + ADDIDATA_MAX_AI + 1 + 1], (unsigned)pw_Value);
						for (b_AdresseCounter = 0 ; b_AdresseCounter < dw_ArgTable[1 + ADDIDATA_MAX_AI + 1 + 1] ; b_AdresseCounter ++)
						{
							printf ("Analog Input value %hu: %hu\n", b_AdresseCounter, *(pw_Value + b_AdresseCounter + dw_ArgTable[1 + ADDIDATA_MAX_AI + 1 + 2]));
						}
					}
				}
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
	uint32_t dw_Size = 0;
	int i_ioctlReturn = 0;

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





	printf("\n=> CMD_xpci3xxx_InitAnalogInputAutoRefresh - Channel 0->3\n");
	{
		uint8_t 	b_ChannelList[4];
		uint8_t 	b_Gain[4];
		uint8_t 	b_Polarity[4];
		uint8_t 	b_SingleDiff = ADDIDATA_DISABLE;
		uint32_t  	dw_NumberOfSequence = 0; // Continuous
		uint32_t dw_ArgTable[(3 * ADDIDATA_MAX_AI) + 6];
		uint8_t	b_ChannelCounter = 0;
		uint8_t    b_DelayMode = 0;
		uint8_t    b_DelayTimeUnit = 0;
		uint16_t    w_DelayTime = 0;

		b_NumberOfChannels = 4;
		b_ChannelList[0] = 0;
		b_ChannelList[1] = 1;
		b_ChannelList[2] = 2;
		b_ChannelList[3] = 3;

		memset(b_Gain, ADDIDATA_1_GAIN, sizeof(b_Gain));
		memset(b_Polarity, ADDIDATA_UNIPOLAR, sizeof(b_Polarity));

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

		i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_InitAnalogInputAutoRefresh,dw_ArgTable);
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

	if ( i_ioctlReturn != 2 )
	{
		printf("\n=> CMD_xpci3xxx_StartAnalogInputAutoRefresh\n");
		{
			uint16_t   w_ArgTable[2];
			uint8_t   b_ConvertTimeUnit = ADDIDATA_MICRO_SECOND;
			uint16_t   w_ConvertTime = 5;

			w_ArgTable[0] = (uint16_t)b_ConvertTimeUnit;
			w_ArgTable[1] = w_ConvertTime;

			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_StartAnalogInputAutoRefresh,w_ArgTable);
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
					printf("\n=> CMD_xpci3xxx_ReleaseAnalogInputAutoRefresh\n");
					{
						uint32_t dw_DummyArg;

						i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReleaseAnalogInputAutoRefresh,&dw_DummyArg);
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
					exit(1);
				}
			}
		}

		printf("\n=> CMD_xpci3xxx_ReadAnalogInputAutoRefreshValueAndCounter - Channel 0->3\n\n");
		{
			uint32_t dw_ArgTable[ADDIDATA_MAX_AI + 1];

			for (dw_Counter = 0 ; dw_Counter < NB_LOOP ; dw_Counter ++)
			{
				i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReadAnalogInputAutoRefreshValueAndCounter,dw_ArgTable);
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
						//exit(1);
					}
				}

				printf("\rValue read : %5u, %5u, %5u, %5u, Counter : %u", dw_ArgTable[0], dw_ArgTable[1], dw_ArgTable[2], dw_ArgTable[3], dw_ArgTable[ADDIDATA_MAX_AI]);
			}

		}


		printf("\n\n=> CMD_xpci3xxx_StopAnalogInputAutoRefresh\n");
		{
			uint32_t dw_DummyArg;

			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_StopAnalogInputAutoRefresh,&dw_DummyArg);
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
					printf("\n=> CMD_xpci3xxx_ReleaseAnalogInputAutoRefresh\n");
					{
						uint32_t dw_DummyArg;

						i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReleaseAnalogInputAutoRefresh,&dw_DummyArg);
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
					exit(1);
				}
			}
		}

		printf("\n=> CMD_xpci3xxx_ReleaseAnalogInputAutoRefresh\n");
		{
			uint32_t dw_DummyArg;

			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReleaseAnalogInputAutoRefresh,&dw_DummyArg);
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

	dw_Size = 0;
	printf("\n=> CMD_xpci3xxx_InitAnalogInputSequence - Channel 0->3\n");
	{
		uint8_t 	b_ChannelList[4];
		uint8_t 	b_Gain[4];
		uint8_t 	b_Polarity[4];
		uint8_t 	b_SingleDiff = ADDIDATA_DISABLE;
		uint32_t  	dw_NumberOfSequence = 4; // Continuous
		uint8_t    b_UseDMA = ADDIDATA_ENABLE;
		uint32_t   dw_NumberOfSequenceForEachInterrupt = 1;
		//			uint32_t  	dw_NumberOfSequence = 0; // Continuous
		//			uint8_t    b_UseDMA = ADDIDATA_DISABLE;
		//			uint32_t   dw_NumberOfSequenceForEachInterrupt = 0;
		uint8_t    b_DelayMode = 0;
		uint8_t    b_DelayTimeUnit = 0;
		uint16_t    w_DelayTime = 0;

		uint32_t dw_ArgTable[(3 * ADDIDATA_MAX_AI) + 8];
		uint8_t	b_ChannelCounter = 0;

		b_NumberOfChannels = 4;
		b_ChannelList[0] = 0;
		b_ChannelList[1] = 1;
		b_ChannelList[2] = 2;
		b_ChannelList[3] = 3;

		memset(b_Gain, ADDIDATA_1_GAIN, sizeof(b_Gain));
		memset(b_Polarity, ADDIDATA_UNIPOLAR, sizeof(b_Polarity));

		dw_ArgTable[0] = (uint32_t)b_NumberOfChannels;

		for (b_ChannelCounter = 0 ; b_ChannelCounter < b_NumberOfChannels ; b_ChannelCounter++)
			dw_ArgTable[1 + b_ChannelCounter] = (uint32_t)b_ChannelList[b_ChannelCounter];

		for (b_ChannelCounter = b_NumberOfChannels ; b_ChannelCounter < (2 * b_NumberOfChannels) ; b_ChannelCounter++)
			dw_ArgTable[1 + b_ChannelCounter] = (uint32_t)b_Gain[b_ChannelCounter - b_NumberOfChannels];

		for (b_ChannelCounter = (2 * b_NumberOfChannels) ; b_ChannelCounter < (3 * b_NumberOfChannels) ; b_ChannelCounter++)
			dw_ArgTable[1 + b_ChannelCounter] = (uint32_t)b_Polarity[b_ChannelCounter - (2 * b_NumberOfChannels)];

		dw_ArgTable[(3 * b_NumberOfChannels) + 1] = (uint32_t)b_SingleDiff;

		dw_ArgTable[(3 * b_NumberOfChannels) + 2] = dw_NumberOfSequence;

		dw_ArgTable[(3 * b_NumberOfChannels) + 3] = (uint32_t)b_UseDMA;

		dw_ArgTable[(3 * b_NumberOfChannels) + 4] = dw_NumberOfSequenceForEachInterrupt;

		dw_ArgTable[(3 * b_NumberOfChannels) + 5] = (uint32_t)b_DelayMode;

		dw_ArgTable[(3 * b_NumberOfChannels) + 6] = (uint32_t)b_DelayTimeUnit;

		dw_ArgTable[(3 * b_NumberOfChannels) + 7] = (uint32_t)w_DelayTime;

		i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_InitAnalogInputSequence,dw_ArgTable);
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
		if (dw_NumberOfSequence == 0)
			dw_Size = dw_NumberOfSequenceForEachInterrupt * b_NumberOfChannels * 2 * 2;
		else
			dw_Size = dw_NumberOfSequence * b_NumberOfChannels * 2;
	}

	if ( i_ioctlReturn != 2 )
	{

		pw_Value = (uint16_t*)mmap (0, dw_Size, PROT_READ, MAP_SHARED, fd, 0);

		//		printf("\n\n=> CMD_xpci3xxx_EnableDisableAnalogInputHardwareTrigger\n");
		//		{
		//			uint32_t dw_ArgTable[4];
		//
		//            uint8_t   b_HardwareTrigger = ADDIDATA_ENABLE;
		//			uint8_t   b_HardwareTriggerLevel = ADDIDATA_HIGH;
		//			uint8_t   b_HardwareTriggerAction = ADDIDATA_TRIGGER_START_A_SEQUENCE_SERIES;
		//			uint32_t dw_HardwareTriggerCount = 1;
		//
		//			dw_ArgTable[0] = b_HardwareTrigger;
		//			dw_ArgTable[1] = b_HardwareTriggerLevel;
		//			dw_ArgTable[2] = b_HardwareTriggerAction;
		//			dw_ArgTable[3] = dw_HardwareTriggerCount;
		//
		//			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_EnableDisableAnalogInputHardwareTrigger,dw_ArgTable);
		//				if (i_ioctlReturn < 0)
		//				{
		//					perror("ioctl");
		//					exit(1);
		//				}
		//				else
		//				{
		//					if ( i_ioctlReturn > 0 )
		//					{
		//					printf("An error occur. Code: %d\n", i_ioctlReturn);
		//					exit(1);
		//					}
		//				}
		//		}


		//		printf("\n\n=> CMD_xpci3xxx_EnableDisableAnalogInputSoftwareTrigger\n");
		//		{
		//			uint8_t   b_ArgTable[2];
		//            uint8_t   b_SoftwareTrigger = ADDIDATA_ENABLE;
		//            uint8_t   b_SoftwareTriggerAction = ADDIDATA_TRIGGER_START_A_SEQUENCE_SERIES;
		//
		//            b_ArgTable[0] = b_SoftwareTrigger;
		//            b_ArgTable[1] = b_SoftwareTriggerAction;
		//
		//			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_EnableDisableAnalogInputSoftwareTrigger,b_ArgTable);
		//				if (i_ioctlReturn < 0)
		//				{
		//					perror("ioctl");
		//					exit(1);
		//				}
		//				else
		//				{
		//					if ( i_ioctlReturn > 0 )
		//					{
		//					printf("An error occur. Code: %d\n", i_ioctlReturn);
		//					exit(1);
		//					}
		//				}
		//		}

		printf("\n=> CMD_xpci3xxx_StartAnalogInputSequence\n");
		{
			uint16_t   w_ArgTable[2];
			uint8_t   b_ConvertTimeUnit = ADDIDATA_MICRO_SECOND;
			uint16_t   w_ConvertTime = 10;

			w_ArgTable[0] = (uint16_t)b_ConvertTimeUnit;
			w_ArgTable[1] = w_ConvertTime;

			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_StartAnalogInputSequence,w_ArgTable);
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
					printf("\n=> CMD_xpci3xxx_ReleaseAnalogInputSequence\n");
					{
						uint32_t dw_DummyArg;

						i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReleaseAnalogInputSequence,&dw_DummyArg);
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
					exit(1);
				}
			}
		}

		/* Wait of IRQ */
		for (dw_Counter = 0 ; dw_Counter < NB_LOOP ; dw_Counter ++)
		{
			printf("\rWait of IRQ");
			//			if (dw_Counter == NB_LOOP/2)
			//				{
			//				printf("\n\n=> CMD_xpci3xxx_AnalogInputSoftwareTrigger\n");
			//				{
			//					uint32_t   dw_DummyArg = 0;
			//
			//					i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_AnalogInputSoftwareTrigger,&dw_DummyArg);
			//						if (i_ioctlReturn < 0)
			//						{
			//							perror("ioctl");
			//							exit(1);
			//						}
			//						else
			//						{
			//							if ( i_ioctlReturn > 0 )
			//							{
			//							printf("An error occur. Code: %d\n", i_ioctlReturn);
			//							exit(1);
			//							}
			//						}
			//				}
			//
			//				printf("\n\n=> CMD_xpci3xxx_GetAnalogInputSoftwareTriggerStatus\n");
			//				{
			//		            uint8_t   b_SoftwareTriggerStatus = 0;
			//
			//					i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_GetAnalogInputSoftwareTriggerStatus,&b_SoftwareTriggerStatus);
			//						if (i_ioctlReturn < 0)
			//						{
			//							perror("ioctl");
			//							exit(1);
			//						}
			//						else
			//						{
			//							if ( i_ioctlReturn > 0 )
			//							{
			//							printf("An error occur. Code: %d\n", i_ioctlReturn);
			//							exit(1);
			//							}
			//						}
			//
			//			   		printf("\nb_SoftwareTriggerStatus = %hu\n", b_SoftwareTriggerStatus);
			//
			//				}
			//
			//				printf("\n\n=> CMD_xpci3xxx_GetAnalogInputHardwareTriggerStatus\n");
			//				{
			//					uint32_t dw_ArgTable[3];
			//		            uint8_t   b_HardwareTriggerStatus = 0;
			//		            uint32_t dw_HardwareTriggerCount = 0;
			//		            uint8_t   b_HardwareTriggerState = 0;
			//
			//					i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_GetAnalogInputHardwareTriggerStatus,dw_ArgTable);
			//						if (i_ioctlReturn < 0)
			//						{
			//							perror("ioctl");
			//							exit(1);
			//						}
			//						else
			//						{
			//							if ( i_ioctlReturn > 0 )
			//							{
			//							printf("An error occur. Code: %d\n", i_ioctlReturn);
			//							exit(1);
			//							}
			//						}
			//		            b_HardwareTriggerStatus = dw_ArgTable[0];
			//		            dw_HardwareTriggerCount = dw_ArgTable[1];
			//		            b_HardwareTriggerState = dw_ArgTable[2];
			//
			//			   		printf("\nb_HardwareTriggerStatus = %hu, dw_HardwareTriggerCount = %5u, b_HardwareTriggerState = %hu\n", b_HardwareTriggerStatus, dw_HardwareTriggerCount, b_HardwareTriggerState);
			//
			//				}
			//
			//				} // end of if (dw_Counter == 50000)
		} // end of for (dw_Counter = 0 ; dw_Counter < NB_LOOP ; dw_Counter ++)

		printf("\n\n=> CMD_xpci3xxx_EnableDisableAnalogInputHardwareTrigger\n");
		{
			uint32_t dw_ArgTable[4];

			uint8_t   b_HardwareTrigger = ADDIDATA_DISABLE;
			uint8_t   b_HardwareTriggerLevel = 0;
			uint8_t   b_HardwareTriggerAction = 0;
			uint32_t dw_HardwareTriggerCount = 0;

			dw_ArgTable[0] = b_HardwareTrigger;
			dw_ArgTable[1] = b_HardwareTriggerLevel;
			dw_ArgTable[2] = b_HardwareTriggerAction;
			dw_ArgTable[3] = dw_HardwareTriggerCount;

			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_EnableDisableAnalogInputHardwareTrigger,dw_ArgTable);
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

		printf("\n\n=> CMD_xpci3xxx_EnableDisableAnalogInputSoftwareTrigger\n");
		{
			uint8_t   b_ArgTable[2];
			uint8_t   b_SoftwareTrigger = ADDIDATA_DISABLE;
			uint8_t   b_SoftwareTriggerAction = 0;

			b_ArgTable[0] = b_SoftwareTrigger;
			b_ArgTable[1] = b_SoftwareTriggerAction;

			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_EnableDisableAnalogInputSoftwareTrigger,b_ArgTable);
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

		printf("\n\n=> CMD_xpci3xxx_StopAnalogInputSequence\n");
		{
			uint32_t dw_DummyArg;

			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_StopAnalogInputSequence,&dw_DummyArg);
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

					printf("\n=> CMD_xpci3xxx_ReleaseAnalogInputSequence\n");
					{
						uint32_t dw_DummyArg;

						i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReleaseAnalogInputSequence,&dw_DummyArg);
						munmap(pw_Value, dw_Size);
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

					exit(1);
				}
			}
		}



		printf("\n=> CMD_xpci3xxx_ReleaseAnalogInputSequence\n");
		{
			uint32_t dw_DummyArg;

			i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_ReleaseAnalogInputSequence,&dw_DummyArg);
			munmap(pw_Value, dw_Size);
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



	// DIGITAL INPUT AND TTL INPUT AND TTL INPUT BI ARE OK
	printf("\n=> CMD_xpci3xxx_SetTTLPortConfiguration - DISABLE - TTL input\n");
	{
		uint8_t b_Arg;

		b_Arg = ADDIDATA_DISABLE; //	configure the bidirectional TTL in Input port

		i_ioctlReturn = ioctl (fd,CMD_xpci3xxx_SetTTLPortConfiguration,&b_Arg);
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
				if ( i_ioctlReturn != 1 )
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
