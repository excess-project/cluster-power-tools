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
  @endverbatim
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
#include <errno.h>
#include <limits.h>
#include <pthread.h>

#include <xpci3xxx.h>

#include "errorlib.h"
#include "findboards.h"
#include "errormsg.h"
#include "input.h"


#define DRIVER_FIFO_STATUS			0
#define DRIVER_FIFO_SOURCE			0
#define ANALOG_INPUT_WITH_DMA			1
#define ANALOG_INPUT_WITHOUT_DMA		2
#define ANALOG_INPUT_FIFO_EMPTY			0xfffffffe
#define ANALOG_INPUT_FIFO_OVERFLOW		0xffffffff

#define ARRAY_SIZE				(1 + ADDIDATA_MAX_AI + 1 + ADDIDATA_MAX_INTERRUPT_ANALOG_INPUT_VALUE)
#define NUMBER_OF_ANALOG_INPUT_DMA_BUFFER	(1 + ADDIDATA_MAX_AI + 1)
#define ANALOG_INPUT_DMA_BUFFER_SIZE		(1 + ADDIDATA_MAX_AI + 1 + 1)
#define ANALOG_INPUT_DMA_BUFFER_POINTER		(1 + ADDIDATA_MAX_AI + 1 + 2)

#define FIFO_SIZE 6000001 // limitation for the sample


typedef struct
{
	uint16_t *pw_DataFIFO;
	uint32_t dw_Read;
	uint32_t dw_Write;
	uint8_t b_OverFlow;
}str_ThreadParams,*pstr_ThreadParams;

str_ThreadParams s_ThreadArgs;

pthread_t thread;


int 		failed_tests = 0;
int 		global_fd = 0;
uint16_t* 	pw_Value = NULL;	
uint32_t	dw_Size = 0;

uint8_t 	b_NumberOfChannels = 0;

static int 	_quit_sample_ = 0;
static int _SequenceStoppedAndReleased_ = 0;
static int _interruptCpt_ = 0;

//------------------------------------------------------------------------------

void quit_sample (int dummy)
{
	if(_SequenceStoppedAndReleased_ == 0)
	{
		_quit_sample_ = 1;

		call_ioctl (global_fd,CMD_xpci3xxx_StopAnalogInputSequence,NULL);

		call_ioctl (global_fd,CMD_xpci3xxx_ReleaseAnalogInputSequence,NULL);

		/* Release the pointer on the shared memory */
		munmap(pw_Value, dw_Size);

		_SequenceStoppedAndReleased_ = 1;
		printf("\n%s _interruptCpt_ = %d\n",__FUNCTION__,_interruptCpt_);

	}
}
	
//------------------------------------------------------------------------------

void FAILED(void) 
{
	printf("failed\n");
	failed_tests++;
}
//------------------------------------------------------------------------------
void *display_datas(void *pArgs)
{
	pstr_ThreadParams ps_ThreadArgs = (pstr_ThreadParams) pArgs;
	uint32_t dw_Counter = 0;
	uint32_t dw_totalNbrOfAcquisition = 0;

	while(_quit_sample_ == 0)
	{

		while((ps_ThreadArgs->dw_Read != ps_ThreadArgs->dw_Write) && (_quit_sample_ == 0))
		{
			if ((dw_totalNbrOfAcquisition % b_NumberOfChannels) == 0)
			{
				// printf ("%6u\t",dw_Counter);
				dw_Counter = dw_Counter + 1;
			}

			// printf ("%6hu\t", ps_ThreadArgs->pw_DataFIFO[ps_ThreadArgs->dw_Read]);
			ps_ThreadArgs->dw_Read = (ps_ThreadArgs->dw_Read+1) % FIFO_SIZE;
			dw_totalNbrOfAcquisition = dw_totalNbrOfAcquisition + 1;
			if ((dw_totalNbrOfAcquisition % b_NumberOfChannels) == 0)
			{
				// printf ("\n");
			}

		}
		if ((ps_ThreadArgs->dw_Read == ps_ThreadArgs->dw_Write) && (ps_ThreadArgs->b_OverFlow == 1))
		{
			printf("Internal sample FIFO overflow\n");
			_quit_sample_ = 1;
		}
	}
	// printf("\n%s _interruptCpt_ = %d\n",__FUNCTION__,_interruptCpt_);
	if(ps_ThreadArgs->pw_DataFIFO != NULL)
	{
		free(ps_ThreadArgs->pw_DataFIFO);
		ps_ThreadArgs->pw_DataFIFO = NULL;
	}

	pthread_exit(NULL);

  
}
//------------------------------------------------------------------------------

void xpci3xxx_signal (int a)
{
	uint32_t dw_ArgTable [ARRAY_SIZE];

	memset (dw_ArgTable, 0 , sizeof (dw_ArgTable));

	/* While value are available (driver fifo is not empty) and not "quit sample" */
	do
	{
		/* Get values and informations about the interrupt type */
		if (call_ioctl (global_fd, CMD_xpci3xxx_TestInterrupt, dw_ArgTable))
			_quit_sample_ = 1;
						
		/* Fifo is empty, no more actions do be done */
		if (ANALOG_INPUT_FIFO_EMPTY == dw_ArgTable [DRIVER_FIFO_STATUS])
			break;
			
		/* Test the interrupt source */
		if (dw_ArgTable [DRIVER_FIFO_SOURCE] == ANALOG_INPUT_WITHOUT_DMA)
		{
			uint8_t b_ChannelCounter = 0;

			for (b_ChannelCounter = 0; b_ChannelCounter <= b_NumberOfChannels; b_ChannelCounter++)
			{
	          	if (b_ChannelCounter == b_NumberOfChannels)
		           	printf ("Analog Input counter : %u\n", dw_ArgTable [b_ChannelCounter + 1]);
	           	else
	           		printf ("Analog Input (initialisation index) %hu value: %u\n", b_ChannelCounter, dw_ArgTable[b_ChannelCounter + 1]);
			}
		}

		/* Test the interrupt source */			
		if (dw_ArgTable [DRIVER_FIFO_SOURCE] == ANALOG_INPUT_WITH_DMA)
		{
			uint8_t b_BufferCpt = 0;
			_interruptCpt_ = _interruptCpt_ + 1;
//			printf("NBR OF BUFFER : %u \n",dw_ArgTable[NUMBER_OF_ANALOG_INPUT_DMA_BUFFER]);

			for(b_BufferCpt = 0; b_BufferCpt < dw_ArgTable[NUMBER_OF_ANALOG_INPUT_DMA_BUFFER];b_BufferCpt ++)
			{
//				ONLY DISPLAY THE FIRST SEQUENCE OF THE BUFFER
//				uint8_t b_ChannelCounter = 0;
//				printf("BUFFER POINTER : %u \n",dw_ArgTable[ANALOG_INPUT_DMA_BUFFER_POINTER + (b_BufferCpt *2)]);
//				printf("BUFFER SIZE : %u \n",dw_ArgTable[ANALOG_INPUT_DMA_BUFFER_SIZE + (b_BufferCpt *2)]);
//				for (b_ChannelCounter = 0; b_ChannelCounter < b_NumberOfChannels; b_ChannelCounter++)
//				{
//		           		printf ("value: %u\t",*(pw_Value + dw_ArgTable[ANALOG_INPUT_DMA_BUFFER_POINTER + (b_BufferCpt *2)] +b_ChannelCounter) );
//				}
//           		printf ("\n");

//				printf("BUFFER POINTER : %u \n",dw_ArgTable[ANALOG_INPUT_DMA_BUFFER_POINTER + (b_BufferCpt *2)]);
//				printf("BUFFER SIZE : %u \n",dw_ArgTable[ANALOG_INPUT_DMA_BUFFER_SIZE + (b_BufferCpt *2)]);

//				SAVE ALL THE VALUES IN THE FIFO OF THE SAMPLE
				uint32_t dw_FIFOFreePlace = 0;
				uint32_t dw_ReadSaved = s_ThreadArgs.dw_Read;

				// Test if enough place in the FIFO
				if(s_ThreadArgs.dw_Write == dw_ReadSaved)
					dw_FIFOFreePlace = FIFO_SIZE - 1;

				if(s_ThreadArgs.dw_Write < dw_ReadSaved)
					dw_FIFOFreePlace = dw_ReadSaved - s_ThreadArgs.dw_Write - 1;

				if(s_ThreadArgs.dw_Write > dw_ReadSaved)
					dw_FIFOFreePlace =  (FIFO_SIZE - s_ThreadArgs.dw_Write) + dw_ReadSaved - 1;


				if(dw_FIFOFreePlace >= dw_ArgTable[ANALOG_INPUT_DMA_BUFFER_SIZE+ (b_BufferCpt *2)])
				{
					printf ("%i\n", dw_ArgTable[ANALOG_INPUT_DMA_BUFFER_SIZE+ (b_BufferCpt *2)]);
					// Copy the all datas in the sample FIFO
					if((s_ThreadArgs.dw_Write + dw_ArgTable[ANALOG_INPUT_DMA_BUFFER_SIZE+ (b_BufferCpt *2)]) < FIFO_SIZE)
					{
						memcpy(&s_ThreadArgs.pw_DataFIFO[s_ThreadArgs.dw_Write],pw_Value + dw_ArgTable[ANALOG_INPUT_DMA_BUFFER_POINTER+ (b_BufferCpt *2)],dw_ArgTable[ANALOG_INPUT_DMA_BUFFER_SIZE+ (b_BufferCpt *2)] * sizeof(uint16_t));
					}
					else
					{
						uint32_t dw_FirstWrite = FIFO_SIZE - s_ThreadArgs.dw_Write;
						memcpy(&s_ThreadArgs.pw_DataFIFO[s_ThreadArgs.dw_Write],pw_Value + dw_ArgTable[ANALOG_INPUT_DMA_BUFFER_POINTER+ (b_BufferCpt *2)],(dw_FirstWrite * sizeof(uint16_t)));
						memcpy(&s_ThreadArgs.pw_DataFIFO[0],pw_Value + dw_ArgTable[ANALOG_INPUT_DMA_BUFFER_POINTER+ (b_BufferCpt *2)] + dw_FirstWrite ,(dw_ArgTable[ANALOG_INPUT_DMA_BUFFER_SIZE+ (b_BufferCpt *2)] - dw_FirstWrite) * sizeof(uint16_t));
					}
					s_ThreadArgs.dw_Write = (s_ThreadArgs.dw_Write + dw_ArgTable[ANALOG_INPUT_DMA_BUFFER_SIZE+ (b_BufferCpt *2)]) % FIFO_SIZE;
				}
				else
				{
					// copy only the number of data
					if((s_ThreadArgs.dw_Write + dw_FIFOFreePlace) < FIFO_SIZE)
					{
						memcpy(&s_ThreadArgs.pw_DataFIFO[s_ThreadArgs.dw_Write],pw_Value + dw_ArgTable[ANALOG_INPUT_DMA_BUFFER_POINTER+ (b_BufferCpt *2)],dw_FIFOFreePlace * sizeof(uint16_t));
					}
					else
					{
						uint32_t dw_FirstWrite = FIFO_SIZE - s_ThreadArgs.dw_Write;
						memcpy(&s_ThreadArgs.pw_DataFIFO[s_ThreadArgs.dw_Write],pw_Value + dw_ArgTable[ANALOG_INPUT_DMA_BUFFER_POINTER+ (b_BufferCpt *2)],(dw_FirstWrite * sizeof(uint16_t)));
						memcpy(&s_ThreadArgs.pw_DataFIFO[0],pw_Value + dw_ArgTable[ANALOG_INPUT_DMA_BUFFER_POINTER+ (b_BufferCpt *2)] + dw_FirstWrite ,(dw_FIFOFreePlace - dw_FirstWrite) * sizeof(uint16_t));
					}
					s_ThreadArgs.dw_Write = (s_ThreadArgs.dw_Write + dw_FIFOFreePlace) % FIFO_SIZE;

					// set the overflow flag
					s_ThreadArgs.b_OverFlow = 1;

				}

			}
		}

		/* Test if FIFO Full */
		if 	(ANALOG_INPUT_FIFO_OVERFLOW == dw_ArgTable[0])
   			printf ("FIFO Overflow\n"); 

	} while ((ANALOG_INPUT_FIFO_EMPTY != dw_ArgTable [DRIVER_FIFO_STATUS]) && (!_quit_sample_));
}

//------------------------------------------------------------------------------

void xpci3xxx_run_test(int fd)
{
	int ret = 0;

	/* clear the data FIFO */
	memset(&s_ThreadArgs,0,sizeof(str_ThreadParams));

	/* allocate the FIFO */
	s_ThreadArgs.pw_DataFIFO = (uint16_t *) malloc(FIFO_SIZE * sizeof(uint16_t));
	if (s_ThreadArgs.pw_DataFIFO == NULL)
	{
	   printf("%s", strerror(ret));
	   exit(EXIT_FAILURE);
	}

	/* install the thread to display the values acquired */
	if ((ret = pthread_create (&thread, NULL, display_datas, (void *)&s_ThreadArgs)) != 0)
	{
	   printf("%s", strerror(ret));
	   exit(EXIT_FAILURE);
	}
	printf ("Create and start thread to display the values acquired\n");



	/* request asynchronous SIGIO */
	{
		int oflags = fcntl(fd, F_GETFL);
		
		printf ("Requesting SIGIO notification\n");
	
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
	
	dw_Size = 0;
	
	{
		uint8_t b_ChannelList[16];
		uint8_t b_Gain[16];
		uint8_t b_Polarity[16];
		uint8_t b_SingleDiff;
		uint32_t dw_NumberOfSequence;
		uint8_t b_UseDMA;
		uint32_t dw_NumberOfSequenceForEachInterrupt = 1;
		uint8_t b_DelayMode;
		uint8_t b_DelayTimeUnit;
		uint16_t w_DelayTime;
		uint32_t dw_ArgTable[(3 * ADDIDATA_MAX_AI) + 8];
		uint8_t	b_ChannelCounter = 0;
	
		printf ("Number of channels to use:\n");
		b_NumberOfChannels = get_integer(1,16);
	
		printf ("Get the polarity to use:\n");
		printf ("     0 : ADDIDATA_BIPOLAR\n");
		printf ("     1 : ADDIDATA_UNIPOLAR\n");
		b_Polarity[0] = get_integer(0,1);
		
		printf ("Get the gain to use:\n");
		printf ("     0 : ADDIDATA_1_GAIN\n");
		printf ("     1 : ADDIDATA_2_GAIN\n");
		printf ("     2 : ADDIDATA_5_GAIN\n");
		printf ("     3 : ADDIDATA_10_GAIN\n");
		b_Gain[0] = get_integer(0,3);
		
		printf ("Get the mode to use:\n");
		printf ("     1 : ADDIDATA_DIFFERENTIAL\n");
		printf ("     0 : ADDIDATA_SINGLE\n");
		b_SingleDiff = get_integer(0,1);
		
		printf ("Use DMA (0: No, 1; Yes):\n");
		b_UseDMA = get_integer(0,1);
		
		if (b_UseDMA == 0)
		{
			printf ("Acquisition mode (0: Continuous, <>0: single mode):\n");
			dw_NumberOfSequence = get_integer(0,65535);
		}
		else
		{
			do
			{
				printf ("Get the number of sequences to acquire (0: Continuous, <>0: single mode) (the result of \"Number of channels\" x \"number of sequences\" has to be even):\n");
				dw_NumberOfSequence = get_integer(0,200000000);
			}
			while (((dw_NumberOfSequence * b_NumberOfChannels) % 2) == 1);
		}
		
		printf ("Number of sequences to do before generating an interrupt:\n");
		dw_NumberOfSequenceForEachInterrupt = get_integer(1,2000000);
		
		printf ("Delay mode:\n");
		printf ("     0 : Delay not used\n");
		printf ("     1 : ADDIDATA_DELAY_MODE_1\n");
		printf ("         The delay is started with the begin of a first sequence.\n");
		printf ("         After this delay, a new acquisition starts.\n");
		printf ("     2 : ADDIDATA_DELAY_MODE_2\n");
		printf ("         The delay is started after the end of a first sequence.\n");
		printf ("         After this delay, a new sequence starts.\n");
		b_DelayMode = get_integer(0,2);
		
		if (b_DelayMode != 0)
		{
			printf ("Delay time unit:\n");
			printf ("     1 : ADDIDATA_MICRO_SECOND\n");
			printf ("     2 : ADDIDATA_MILLI_SECOND\n");
			printf ("     3 : ADDIDATA_SECOND\n");
			b_DelayTimeUnit = get_integer(1,3);
			
			printf ("Delay time value:\n");
			w_DelayTime = get_integer(1,65535);
		}				
		
		{
			int i;
			for (i=0; i < b_NumberOfChannels; i++) 
			{
				b_ChannelList[i] = i;
				b_Polarity[i] = b_Polarity[0];
				b_Gain[i] = b_Gain[0];
			}
		}
		
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
				
		if (call_ioctl (fd,CMD_xpci3xxx_InitAnalogInputSequence,dw_ArgTable))
			goto stop;
			
		printf ("CMD_xpci3xxx_InitAnalogInputSequence\n");			
			
		if (dw_NumberOfSequence == 0)
			dw_Size = dw_NumberOfSequenceForEachInterrupt * b_NumberOfChannels * 2 * 2;
		else
			dw_Size = dw_NumberOfSequence * b_NumberOfChannels * 2;
	}
	printf("Try to get a shared memory with mmap\n");

	/* Open and get a pointer on the shared memory */		
	pw_Value = (uint16_t*)mmap (0, dw_Size, PROT_READ, MAP_SHARED, fd, 0);
		
	if (pw_Value == MAP_FAILED)
	{
		printf("\n mmap failed\n");
		goto stop;
	}
	
	{
		uint16_t   w_ArgTable[2];
		
		printf ("Conversion time unit:\n");
		printf ("     1 : ADDIDATA_MICRO_SECOND\n");
		printf ("     2 : ADDIDATA_MILLI_SECOND\n");
		w_ArgTable[0] = get_integer(1,2);
		
		printf ("Conversion time value:\n");
		if (w_ArgTable[0] == 1)
			w_ArgTable[1] = get_integer(5,65535);			
		
		if (w_ArgTable[0] == 2)
			w_ArgTable[1] = get_integer(1,65535);
		
		if (call_ioctl (fd,CMD_xpci3xxx_StartAnalogInputSequence,w_ArgTable))
			goto release;
			
		printf ("CMD_xpci3xxx_StartAnalogInputSequence\n");		
	}
	
	/* Wait of IRQ */
	while (!_quit_sample_)
		sleep (1);

stop:			
	/* If stop without Ctrl + C */
	if (!_quit_sample_)
		call_ioctl (fd,CMD_xpci3xxx_StopAnalogInputSequence,NULL);
	
release:				
			/* If stop without Ctrl + C */
	if (!_quit_sample_)
		call_ioctl (fd,CMD_xpci3xxx_ReleaseAnalogInputSequence,NULL);
	
	/* If stop without Ctrl + C */
	if (!_quit_sample_)
	{		
		/* Release the pointer on the shared memory */
		if (pw_Value != MAP_FAILED)        
			munmap(pw_Value, dw_Size);
	}
	_quit_sample_ = 1;
}

//------------------------------------------------------------------------------

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
	
	printf ("This sample demonstrates how to use sequence mode in user mode.\n");
	printf ("If you need to make fast acquisitions (acquisitions < ms),\n");
	printf ("we advice you to show kernel mode samples.\n");
	
	printf ("Configuration parameters are to be set according to possibilities of the board you are using.\n");
	printf ("After measure start, use Ctrl+C to stop the sample\n");
		
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

//------------------------------------------------------------------------------
