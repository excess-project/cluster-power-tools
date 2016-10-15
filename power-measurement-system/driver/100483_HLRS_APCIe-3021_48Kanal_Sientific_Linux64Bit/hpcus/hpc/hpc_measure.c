/**
 * hpc_measurement.c - This source code defines the process to record 
 * the output of the A/D converters APCIe-3021. Each of the converters
 * has eight analog channels. This process configures and records each
 * of the channels of several APCIe-3021s cards. Currently, its support
 * four cards. If you install more or less ACPIe-3012 change its
 * number in the header hpc.h. The can start several process parallel,
 * each for one of cards. 
 * Consider, that it is not allowed
 * to record the same card simultaneously from different processes (At least
 * not tested). The configure file is divided in the GLOBAL part and in 
 * the channel definition parts for each of the installed card.
 * The frequency of the A/D converter is 100kHz, which is shared between the
 * eight channels. The frequency can be changed in the field "acquisition_time".
 * The process write out the recordered values after an interrupt. The interval
 * can be defined in the field "number_of_sequences_per_interrupt". In case, that
 * all eight channels are on and the frequency equals to 100kHz (acquisition_time=
 * 10), the number 12500 in that field let to produce the outpu in each second.
 * The process hpc_measure saves the raw data in the unformated format (32-bit
 * floating point numbers). After each interrupt also the timestamp will be written out
 * in a separate file in the unformated format (64 bit unsighed int). One record
 * consists on two numbers: seconds and nonoseconds of interrupt occuriency.
 * The field ac_voltage_channel_id is used only if you measure also the AC power
 * consumption with help of additional devices to record the voltage profil.
 * The field  "duration_time" let you to stop the process after a certain
 * interval. Use 0 for the continues mode. The process with the number pid can 
 * be stoped through the signal  * from the bash: "kill -SIGINT pid" 
 * You are able to turn on/off each of the channels throught
 * the use of the field "channel_0_on_off". 
 * In case, you would like to carry out the measurements with the several cards and
 * need the synchronisation between each paar of the channels use the master bus, which
 * connect the cards and set the field "trigger_on_off" to 1. The drift between the channels
 * will be 10 nansoseconds.
 * There are also the parameters for the channels of the card.  Depending on the
 * maximal voltage on the input, chooce the appropriate gain in the field "channel_0_gain":
 * 10 is for the voltage range [0;1] (V) in Bipolar mode and [-1;1] (V) in Polar mode.
 * The channel coefficients  are defined in the configuration file. For example,
 * (see ../profiles/config_node02.ini for the whole definition of the channels)
 *[GLOBAL]
 *acquisition_time			= 10		; In microseconds, min.: 10 us, max.: 65535 us
 *number_of_sequences_per_interrupt	= 12500; The interrupt minimal time should be set at 1 second,.. (Max.: 524287 with 8 channels)
 *total_sequence_count			= 0			; 0: Continuous mode
 *;Directory + file name should not exceed 256 characters
 *log_file_path						= /ext/node02/  ; This field is not used
 *raw_file_path						= /raw_data/node02/; directory to save the files with the raw_data
 *ac_voltage_channel_id					= 28	; channel_id for current profile in ac circuit
 *ac_num_periods					= 1	; to calculate N for average of the total power (Power_avrg= [1/N*Sum(V(i)*A(i)); 0<iN])
 *duration_time						= 0	; Measure duration in seconds (0: disabled)
 *trigger_on_off					= 0	; 0: Disable / 1: enable the trigger

 *[BOARD_0]
 *channel_0_id		= 0			; unique id sorted ascending
 *channel_0_label	= CPU1_(A)_node01	; Label of channel
 *channel_0_on_off	= 1			; 0: Off / 1: On
 *channel_0_gain	= 3			; 0: Gain 1 / 1: Gain 2 / 2: Gain 5 / 3: Gain 10 
 *channel_0_polarity	= 1			; 1: Unipolar / 0: Bipolar
 *channel_0_coeff1	= 100.0			; measured value = measured value * coeff1
 *channel_0_coeff2	= 0.0			; measured value = measured value + coeff2
 *
 *channel_1_id		= 1                     ;
 *channel_1_label	= CPU1_(V)_node01	; Label of channel
 * .......
 *
 *
 * This is a free Software.
 * Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
 *
 * Modifications of the original source code were done by
 * the High Performance Computing Center Stuttgart (HLRS)
 * University of Stuttgart
 * Nobelstraße 19
 * 70569 Stuttgart
 * Germany
 * Bugs to:khabi@hlrs.de
 **/

/** @file hpcus.c
 *
 * @brief This is the main code of the High Performance Computing application.
 *
 * @author J. Krauth
 *
 * @par LICENCE
 * @verbatim

   Copyright (C) 2014  ADDI-DATA GmbH for the source code of this module.

   ADDI-DATA GmbH
   Airpark Business Center
   Airport Boulevard B210
   77836 Rheinmuenster
   Germany
   Tel: +49(0)7229/1847-0
   Fax: +49(0)7229/1847-200
   http://www.addi-data-com
   info@addi-data.com

 * @endverbatim
 *
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <string.h>
#include <sys/sysmacros.h> /* major() minor() */
#include <signal.h>
#include <sys/mman.h>
#include <syslog.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <xpci3xxx.h>

#include "hpc_config.h"
#include "hpc_jitter.h"
#include "hpc_signals.h"
#include "hpc_utils.h"
#include "hpc_threads.h"
#include "hpc.h"

#include "errorlib.h"
#include "findboards.h"
#include "errormsg.h"
#include "input.h"
#include "revision.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/** Global variable to inform all tasks
 * that the program has to quit.
 */
int stop = APPLICATION_RUN;
/**DKH - Mutex for global variable stop
 * */ 
pthread_mutex_t stop_mutex=PTHREAD_MUTEX_INITIALIZER;

/** Global structure that contains
 * informations necessary to all
 * tasks.
 */
struct hpc_t hpc;


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int main (int argc, char** argv)
{
	int board = 0;								/**< board (minor number) to use / configure */
	struct threadParams_t threadParams[4];		/**< Thread parameters */
	pthread_t thread[4] = {0};						/**< Threads IDs */
	int quit = 0;
	int useBoards = 0;
    int stop_local;

	memset (&hpc, 0, sizeof (struct hpc_t));

	/* Check the command line parameters
	 * There should be a hpc.config.file name or path.
	 */
	if (argc < 2)
	{
		printf ("%s %s\n", argv[0], __SVN_PROJECT_REV_);
		printf ("Use %s CONFIG.INI\n", argv[0]);
		return EXIT_FAILURE;
	}

	/* We are logging errors in the linux log file
	 * that can be displayed with the command "dmesg"
	 * or "cat /var/log/messages"
	 */
	openlog (argv[0], LOG_NDELAY | LOG_PERROR, LOG_USER);

  
	/* Attach a function to the kill signal.
	 * kill is raised when Ctrl + c keys are used.
	 * */
	if (hpc_set_signals (signal_handler) < 0)
		goto error;

	/* Check if boards are detected and how many */
	hpc.nBoards = apci_find_boards("xpci3xxx", &hpc.node);
	if (hpc.nBoards <= 0)
		goto error;

	/* Set the maximal number of boards to use */
	if (hpc.nBoards > NUMBER_OF_BOARDS)
		hpc.nBoards = NUMBER_OF_BOARDS;

	loginfo_info ("%i %s been detected (this program is able to manage up to %i boards)", hpc.nBoards, (hpc.nBoards > 1)?"boards have":"board has", NUMBER_OF_BOARDS);

	/* Read the INI configuration file */
	if (hpc_read_config_file (argv[1], &hpc.config, hpc.nBoards))
		goto error;

	/* Create the directory in which to save the *.dat files. */
	if (mkdir (hpc.config.raw_file_path, 0777) < 0)
	{
		switch (errno)
		{
			case EEXIST:
			break;

			default:
				loginfo_error ("Fail to create %s directory (mkdir %s)", hpc.config.raw_file_path, strerror (errno));
                pthread_mutex_lock(&stop_mutex);
				stop = APPLICATION_QUIT;
                pthread_mutex_unlock(&stop_mutex);
		}
	}

	/* Initialize boards by using configuration informations */
	{
		int index = 0;
		uint32_t arg [(3 * ADDIDATA_MAX_AI) + 8];

		/* For all boards */
		for (board=0; board<hpc.nBoards; board++)
		{
			/* If no channels are selected, the board is not to be used */
			if (!hpc.config.board[board].nChannels)
				continue;

			useBoards++;

			/* Set the number of channels in the sequence */
			arg[0] = hpc.config.board[board].nChannels;

			/* Configure the sequence index, the gain and the polarity */
			for (index=0; index<arg[0]; index++)
			{
				arg[1 + index] = hpc.config.board[board].sequence[index];
				arg[1 + index + arg[0]] = hpc.config.board[board].gain[index];
				arg[1 + index + (arg[0] * 2)] = hpc.config.board[board].polarity[index];
			}

			arg[(3 * arg[0]) + 1] = 1; // Differential mode
			arg[(3 * arg[0]) + 2] = hpc.config.total_sequence_count;
			arg[(3 * arg[0]) + 3] = 1; // Enable the DMA
			arg[(3 * arg[0]) + 4] = hpc.config.number_of_sequences_per_interrupt;
			arg[(3 * arg[0]) + 5] = 0; // Delay mode
			arg[(3 * arg[0]) + 6] = 0; // Delay time unit
			arg[(3 * arg[0]) + 7] = 0; // Delay time

			/* Initialize the jitter measure:
			 * A hardware timer, from the board, starts to count
			 * once a DMA interrupt is generated. It counts the number
			 * of elapsed microseconds. In the interrupt function
			 * of the boards, the elapsed number of microseconds is read.
			 * This is the jitter time between the end of the acquisition
			 * and the DMA interrupt generation. It it is subtract from
			 * the computer time to compute the time-stamp.
			 */
             
			if (hpc_initialize_jitter_measure (hpc.node[board]))
				goto error;

			/* Initialize the board */
			if (call_ioctl (hpc.node[board], CMD_xpci3xxx_InitAnalogInputSequence, arg))
				goto error;

			/* We use a shared memory to access the DMA values */

			/* Compute the memory size to allocate for the shared memory. */
			if (hpc.config.total_sequence_count == 0)
				hpc.sharedMemSize[board] = (hpc.config.number_of_sequences_per_interrupt * hpc.config.board[board].nChannels * 2 * 2);	// If continuous mode
			else
				hpc.sharedMemSize[board] = (hpc.config.total_sequence_count * hpc.config.board[board].nChannels * 2);					// If single mode

			/* Open and get a pointer on the shared memory */
			hpc.sharedMem[board] = (uint16_t*) mmap (0, hpc.sharedMemSize[board], PROT_READ, MAP_SHARED, hpc.node[board], 0);

			if (hpc.sharedMem[board] == MAP_FAILED)
			{
				loginfo_error ("Fail to allocate the shared memory (mmap)");
				goto error;
			}

			/* Enable the hardware trigger */
			if (hpc.config.trigger_on_off)
				arg[0] = ADDIDATA_ENABLE;			// Enable the hardware trigger
			else
				arg[0] = ADDIDATA_DISABLE;			// Enable the hardware trigger

			arg[1] = ADDIDATA_HIGH;					// Trigger on the high level
			arg[2] = ADDIDATA_ONE_SHOT_TRIGGER;
			arg[3] = 1;								// Trigger after the first high level signal

			if (call_ioctl (hpc.node[board], CMD_xpci3xxx_EnableDisableAnalogInputHardwareTrigger, arg))
				goto error;

			{
				uint16_t arg[2] = {0};

				arg[0] = 1; // Microseconds
				arg[1] = hpc.config.acquisition_time;

				if (call_ioctl (hpc.node[board], CMD_xpci3xxx_StartAnalogInputSequence, arg))
					goto error;
			}

			/* Initialize  the thread structure */
			memset (&threadParams[board], 0, sizeof(struct threadParams_t));
			memcpy (&threadParams[board].hpc, &hpc, sizeof(struct hpc_t));
			threadParams[board].board = board;

			/* Create the thread to manage acquired values */
			if (pthread_create (&thread[board], NULL, data_management, (void *)&threadParams[board]) != 0)
			{
				loginfo_error ("Data management thread creation for board %i error (pthread_create): %s\n", board, strerror(errno));
				goto error;
			}
		}
	}

	{
		/* On each boards the first digital input (the trigger input)
		 * are connected to synchronize the acquisition start. */
		uint32_t arg = 0xF;

		if (call_ioctl (hpc.node[0], CMD_xpci3xxx_Set32DigitalOutputsOn, &arg))
			goto error;

		/* If the program has to stop automatically after a
		 * specified duration time, set an alarm.
		 */
		if (hpc.config.duration_time != 0)
		{
			if (alarm (hpc.config.duration_time))
			{
				loginfo_error ("Fail to set the alarm to end the application (alarm): %s\n", strerror(errno));
				goto error;
			}
		}
	}

	/* Wait until the application is stopped or working time is reached */
    pthread_mutex_lock(&stop_mutex);
    stop_local=stop;
    pthread_mutex_unlock(&stop_mutex);
	while (stop_local)
	{
		/* For all possible boards */
		for (board=0; board<hpc.nBoards; board++)
		{
			/* If the board is used */
			if (hpc.config.board[board].nChannels)
			{
				/* Is the thread stopped */
				if ((thread[board] != 0) && (pthread_tryjoin_np(thread[board], NULL) != EBUSY))
				{
					thread[board] = 0;
					quit++;
				}
			}
		}

		/* If all board treads are stopped */
		if (quit == useBoards)
        {
            pthread_mutex_lock(&stop_mutex);
            stop = APPLICATION_QUIT;
            pthread_mutex_unlock(&stop_mutex);
            pthread_mutex_lock(&stop_mutex);
            stop_local=stop;
            pthread_mutex_unlock(&stop_mutex);
        }
	}

error:

	/* Release all */
	for (board=0; board<hpc.nBoards; board++)
	{
		/* If no channels are selected, the board is not to be used */
		if (!hpc.config.board[board].nChannels)
			continue;

		/* Stop acquisitions */
		call_ioctl (hpc.node[board], CMD_xpci3xxx_StopAnalogInputSequence, NULL);
		call_ioctl (hpc.node[board], CMD_xpci3xxx_ReleaseAnalogInputSequence, NULL);

		/* Reset the trigger output */
		{
			uint32_t arg = 0;
			call_ioctl (hpc.node[board], CMD_xpci3xxx_Set32DigitalOutputsOn, &arg);
		}

		/* Stop the jitter measure */
        /*DKH
		hpc_release_jitter_measure (hpc.node[board]);
		loginfo_info("hpc_release_jitter_measure done");*/
		/* Unmap the shared memory */
		if ((hpc.sharedMem[board] != MAP_FAILED) && (hpc.sharedMem[board] != 0) && (hpc.sharedMemSize[board] != 0))
			munmap (hpc.sharedMem[board], hpc.sharedMemSize[board]);
		else
		  loginfo_error ("Fail to release the shared memory (mmap)");

		loginfo_info("Unmap the shared memory done");
	}
    /* Release stop mutex*/
    if ( pthread_mutex_destroy(&stop_mutex) < 0 )
    {
        loginfo_error ("Fail to destroy the stop mutex.");
    }
	closelog ();
    pthread_mutex_lock(&stop_mutex);
    stop = APPLICATION_QUIT;
    pthread_mutex_unlock(&stop_mutex);
    pthread_mutex_destroy(&stop_mutex);
	return 0;
}

//------------------------------------------------------------------------------
