/** @file hpc_threads.c
 *
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
#include <errno.h>
#include <stdint.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <xpci3xxx.h>

#include "hpc_config.h"
#include "hpc_jitter.h"
#include "hpc_signals.h"
#include "hpc_threads.h"
#include "hpc_utils.h"
#include "hpc.h"

#include "errorlib.h"
#include "findboards.h"
#include "errormsg.h"
#include "input.h"

#define __THREAD_SCHED_ 	SCHED_FIFO
#define __MAIN_SCHED_ 		SCHED_RR

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/** Affect the calling thread to the selected CPU core.
 *
 * @param[in] core: ID of the CPU core to use.
 *
 * @retval 0: No error.
 * @retval <>0: Error.
 */
int affect_to_core (int core)
{
	cpu_set_t cpuset;
	int ret = 0;
	pthread_t current_thread = pthread_self();

	if ((core < 0) || (core >= sysconf (_SC_NPROCESSORS_ONLN)))
		return EINVAL;

	CPU_ZERO (&cpuset);
	CPU_SET (core, &cpuset);

	ret = pthread_setaffinity_np (current_thread, sizeof (cpu_set_t), &cpuset);
	if (ret != 0)
		return ret;

    ret = pthread_getaffinity_np (current_thread, sizeof (cpu_set_t), &cpuset);
	if (ret != 0)
		return ret;

    if (!CPU_ISSET (core, &cpuset))
    	loginfo_error ("Acquisition thread of board %i is not set to cpu %i", core, core);

    return 0;
}

//------------------------------------------------------------------------------

int set_pthread_scheduling_policy (pthread_t thread, int policy)
{
	int ret = 0;
	int priority = 0;
	struct sched_param param;

	if ((priority = sched_get_priority_max (policy)) < 0)
	{
		loginfo_error ("sched_get_priority_max %d : %s\n", ret, strerror(ret));
		return 1;
	}

	param.sched_priority = priority;

	/* Get the thread policy */
	if ((ret = pthread_setschedparam (thread, policy, &param)) != 0)
	{
		loginfo_error ("pthread_setschedparam %d : %s\n", ret, strerror(ret));
		return 1;
	}

	return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void *data_management (void *args)
{
	struct threadParams_t *param = (struct threadParams_t *) args;
	uint32_t arg[ARRAY_SIZE];
	int board = param->board;
	unsigned long counter = 0;
	uint32_t nBuff = 0;
	FILE *fd = NULL;
	int ret = 0;
	char name[256] = {0};
	struct fileHeader_t header;
        int stop_local;
	memset (arg, 0 , sizeof (arg));

	/* Each threads get it's own CPU */
	affect_to_core (board);

	/* Set the application with soft real-time priority */
//	set_pthread_scheduling_policy (pthread_self(), __THREAD_SCHED_);

	/* Until the user stop the application
	 *
	 * This loop will save data in a raw file
	 * formated as following:
	 *
	 * Header:
	 * 	  Name		Type				Remark
	 * 	- size		uint16_t			The number of data in bytes.
	 * 	- board		uint8_t				Minor number of the board.
	 * 	- timeval	struct timepsec		The time-stamp.
	 *  - config	struct iniConfig_t	Configuration informations from the INI file
	 *
	 * Data:
	 * 	  Name		Type				Remark
	 * 	- Data		uint16_t			One data is coded on 16 bit.
	 */
    pthread_mutex_lock(&stop_mutex);
    stop_local = stop;
    pthread_mutex_unlock(&stop_mutex);
	while (stop_local)
	{
		/* Get values and informations about the interrupt type */
		if (call_ioctl (param->hpc.node[board], CMD_xpci3xxx_TestInterrupt, arg))
			goto error;

		/* FIFO is empty, do nothing */
		if (arg [DRIVER_FIFO_STATUS] == ANALOG_INPUT_FIFO_EMPTY)
        {
            pthread_mutex_lock(&stop_mutex);
            stop_local = stop;
            pthread_mutex_unlock(&stop_mutex);
			continue;
        }

		/* FIFO is full, generate an error */
		if (arg [DRIVER_FIFO_STATUS] == ANALOG_INPUT_FIFO_OVERFLOW)
		{
			loginfo_error ("Board %i fifo is full", board);
			goto error;
		}

		/* No DMA interrupt, do nothing */
		if (arg [DRIVER_FIFO_SOURCE] != ANALOG_INPUT_WITH_DMA)
        {
            pthread_mutex_lock(&stop_mutex);
            stop_local = stop;
            pthread_mutex_unlock(&stop_mutex);
			continue;
        }

		/* Open the raw file
		 * The file name is composed of:
		 * - the path where it has to be saved
		 * - the board minor number
		 * - a counter
		 *
		 * Remark: Existing files are overwritten.
		 */
		sprintf (name, "%s/%i_%lu.raw.tmp", param->hpc.config.raw_file_path, board, counter);
		fd = fopen(name, "w+");
		if (fd == NULL)
		{
			loginfo_error ("Fail to create %s file (fopen %s)", name, strerror (errno));
			goto error;
		}

		/* Read all data from the shared memory */
		for (nBuff = 0; (nBuff < arg[NUMBER_OF_ANALOG_INPUT_DMA_BUFFER]) && stop_local; nBuff++)
		{
			/* Get the pointer on DMA data
			 * sharedMem id the base DMA address
			 * ANALOG_INPUT_DMA_BUFFER_POINTER is the offset at which current data are to be read */
			void * data = (((uint16_t *) param->hpc.sharedMem[board]) + arg[ANALOG_INPUT_DMA_BUFFER_POINTER + (nBuff * 4)]);

			/* Fill the header informations */

			/* The size is in bytes */
			header.size = (arg[ANALOG_INPUT_DMA_BUFFER_SIZE + (nBuff * 4)] * sizeof (uint16_t));
			header.board = board;
			header.timestamp.tv_sec = (long) ((long)arg[ANALOG_INPUT_DMA_BUFFER_POINTER + 3] | (long)((long)arg[ANALOG_INPUT_DMA_BUFFER_POINTER + 4] << 32));
			header.timestamp.tv_nsec = (long) ((long)arg[ANALOG_INPUT_DMA_BUFFER_POINTER + 1] | (long)((long)arg[ANALOG_INPUT_DMA_BUFFER_POINTER + 2] << 32));

			/* Copy the INI file param->hpc.configuration */
			memcpy (&header.config, &param->hpc.config, sizeof (param->hpc.config));

			/* Save the header in the raw file */
			ret = fwrite (&header, sizeof (header), 1, fd);

			if ((ret < 0) || (ret != 1))
			{
				loginfo_error ("Fail to write the header in a file (ret = %i, fwrite %s)",ret, strerror (errno));
				goto error;
			}

			/* Save data in the raw file */
			ret = fwrite (data, header.size, 1, fd);

			if ((ret < 0) || (ret != 1))
			{
				loginfo_error ("Fail to write data in a file (ret = %i, fwrite %s)",ret, strerror (errno));
				goto error;
			}

			loginfo_info ("%s board: %i data bytes: %i counter: %lu", __FUNCTION__, board, header.size, counter);
            pthread_mutex_lock(&stop_mutex);
            stop_local = stop;
            pthread_mutex_unlock(&stop_mutex);
		}

		/* We can force the file to be written physically on the disc */
		// fsync (fileno (fd));
		fclose (fd);
		fd = NULL;

		/* Rename the raw file to inform
		 * the conversion application that the
		 * file is ready to be treated.
		 */
		{
			char goal[256];
			sprintf (goal, "%s/%i_%lu.raw", param->hpc.config.raw_file_path, board, counter);
			rename (name, goal);
		}

		counter++;
        pthread_mutex_lock(&stop_mutex);
        stop_local = stop;
        pthread_mutex_unlock(&stop_mutex);
		/* Stop the acquisition once the number of sequences is reach */
		if ((param->hpc.config.total_sequence_count != 0) && ((counter * param->hpc.config.number_of_sequences_per_interrupt) >= param->hpc.config.total_sequence_count))
			break;
	}

	/* If the number of sequences is reach */
	if (fd)
		fclose (fd);

	fd = NULL;
    pthread_mutex_lock(&stop_mutex);
	stop = APPLICATION_QUIT;
    pthread_mutex_unlock(&stop_mutex);
	pthread_exit (NULL);

	/* If there is an error (stop the program) */
error:

	if (fd)
		fclose (fd);

	fd = NULL;
    pthread_mutex_lock(&stop_mutex);
	stop = APPLICATION_QUIT;
    pthread_mutex_unlock(&stop_mutex);


	pthread_exit (NULL);
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

