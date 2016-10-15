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

#include <stdint.h>
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
#include <syslog.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <dirent.h>
#include <math.h>

#include "hpc_config.h"
#include "hpc_signals.h"
#include "hpc_utils.h"
#include "hpc.h"
#include "revision.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/** Global variable to inform all tasks
 * that the program has to quit.
 */
int stop = APPLICATION_RUN;

/** Global structure that contains
 * informations necessary to all
 * tasks.
 */
struct hpc_t hpc;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/** Substract two timespec struct.
 *
 * @param[out] result: Result of (x - y).
 * @param[in] x: Timespec struct from which to subtract y.
 * @param[in] y: Timespec struct to subtract from x.
 *
 * @retval 0: No error.
 * @retval 1: result is negative.
 */
int timespec_substract (struct timespec *result, struct timespec *x, struct timespec *y)
{
	/* Check if x <= y */
	if ((x->tv_sec < y->tv_sec) || ((x->tv_sec == y->tv_sec) && (x->tv_nsec <= y->tv_nsec)))
	{
		result->tv_sec = 0;
		result->tv_nsec = 0;

		return 1;
	}
	else
	{
		/* When x > y */
		result->tv_sec = (x->tv_sec - y->tv_sec);

		if (x->tv_nsec < y->tv_nsec)
		{
			result->tv_nsec = (x->tv_nsec + 1000000000L - y->tv_nsec);
			result->tv_sec--;
		}
		else
		{
			result->tv_nsec = x->tv_nsec - y->tv_nsec;
		}
	}

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/** Filter function for scandir.
 *
 * scandir will only check *.raw files.
 *
 * @param[in] dir: Directory entry to check.
 *
 * @retval 0: No file found.
 * @retval 1: File found.
 */
int filter (const struct dirent * dir)
{
	const char *s = dir->d_name;
	int len = strlen(s) - 4;

	if(len >= 0)
		if (strncmp (s + len, ".raw", 4) == 0)
			return 1;

	return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/** Get the name of the oldest raw file.
 *
 * @param[in] dir: Path in which are the raw files.
 * @param[out] fname: Name of the oldest raw file.
 *
 * @retval >-1: Number of files.
 * @retval -1: Error.
 */
int get_raw_file_name (char *dir, char *fname)
{
	int i = 0;
	int nfiles = 0;
	struct dirent **filelist = {0};

	/* Get the number, and names, of raw files to convert.
	 * The oldest is always converted in first.
	 */
	nfiles = scandir (dir, &filelist, filter, versionsort);
	if (nfiles == 0)
		return 0;

	if (nfiles < 0)
	{
		loginfo_error ("Fail to get files list (scandir %s)", strerror (errno));
		return -1;
	}

	strcpy (fname, filelist[0]->d_name);

	for (i=0; i<nfiles; i++)
	{
		if (filelist[i]);
		{
			free (filelist[i]);
		}
	}

	if (filelist)
		free (filelist);

	return nfiles;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

/** Create the time-stamp file.
 *
 * @param[in] header: The header structure.
 *
 * @retval 0: No error.
 * @retval 1: Error.
 */
int create_timestamp_file (struct fileHeader_t header)
{
	FILE *fdtime = NULL;
	char timefname[256] = {0};
	int ret = 0;

	/* Open the file to save the time-stamp */
	sprintf (timefname, "%s/Time_%u.dat", header.config.log_file_path, header.board);
	fdtime = fopen (timefname, "a+");
	if (fdtime < 0)
	{
		loginfo_error ("Fail to create %s file (open %s)", timefname, strerror (errno));
		return 1;
	}

	ret = fwrite (&header.timestamp, sizeof (struct timespec), 1, fdtime);

	if ((ret < 0) || (ret != 1))
	{
		loginfo_error ("Fail to write the time-stamp in the file (fwrite %s)", strerror (errno));
		return 1;
	}

	fclose (fdtime);

	return 0;
}

//------------------------------------------------------------------------------

/** Make computations on values.
 *
 * This function can be edited to add other computations.
 *
 * @param[in] polarity: 1: Unipolar, other bipolar.
 * @param[in] gain: 0: Gain 1 / 1: Gain 2 / 2: Gain 5 / 3: Gain 10.
 * @param[in] data: The raw value.
 * @param[in] coeff1: The coefficient to multiply with the value.
 * @param[in] coeff2: The coefficient to add with the value.
 */
static inline void compute (uint8_t polarity, uint32_t gain, uint16_t data, float coeff1, float coeff2, float *fData)
{
	float voltage = 10.0;

	switch (gain)
	{
		default:
		case 0:
			voltage = 10.0;
		break;

		case 1:
			voltage = 5.0;
		break;

		case 2:
			voltage = 2.0;
		break;

		case 3:
			voltage = 1.0;
		break;
	}

	/* Check if the value is an unipolar value */
	if (polarity == 1)
		*fData = ((voltage / 65535.0) * data);			// Conversion in volts (unipolar)
	else
		*fData = (((voltage * 2) / 65535.0) * data) - voltage;	// Conversion in volts (bipolar)

	*fData *= coeff1;
	*fData += coeff2;
}

void set_statistic_to_zero(double* over_sum,double* average,double* deviation,double* variance,double* ep, int num_channels)
{
	int jj;

	for(jj=0;jj<num_channels;jj++)
	{
		over_sum[jj]=0.0;
		average[jj]=0.0;
		deviation[jj]=0.0;
		variance[jj]=0.0;
		ep[jj]=0.0;
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int main (int argc, char** argv)
{
	int ret = 0;
	FILE *fddat[8] = {NULL};
	FILE *fdraw = NULL;
	char dstfname[256] = {0};
	char srcfname[256] = {0};
	char buf[256] = {0};
	struct fileHeader_t header;
	int nChannels = 0;
	int channel = 0;
	int size = 0;
	int channelDataIndex = 0;
	int nData = 0;
	float *fData[8] = {NULL};
	uint16_t * data = NULL;
	int nDataPerChannel = 0;
	int i = 0;
	int nfiles = 0;
        double over_sum[8];
        double average[8];
        double deviation[8];
        double variance[8];
	double ep[8];
	double temp_double;
	// struct timespec result; // (only for test)
	// struct timespec timespecOld[4]; // (only for test)

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

	/* Check the command line parameters
	 * There should be the path in which raw files
	 * are saved.
	 */
	if (argc < 2)
	{
		printf ("%s %s\n", argv[0], __SVN_PROJECT_REV_);
		printf ("Use %s RAW FILES PATH\n", argv[0]);
		return EXIT_FAILURE;
	}

	while (stop)
	{
		/* Get the number, and names, of raw files to convert.
		 * The oldest is always converted in first.
		 */
		nfiles = get_raw_file_name (argv[1], buf);

		/* No files to convert, loop again */
		if (nfiles == 0)
			continue;

		/* Can't get files, error */
		if (nfiles < 0)
			goto error;

		/* Open the raw file */
		sprintf (srcfname, "%s/%s", argv[1], buf);
		fdraw = fopen (srcfname, "r");
		if (fdraw == NULL)
		{
			loginfo_error ("Fail to open %s file (fopen %s)", srcfname, strerror (errno));
			goto error;
		}

		/* Read the raw file header */
		ret = fread (&header, sizeof (header), 1, fdraw);

		if ((ret < 0) || (ret != 1))
		{
			loginfo_error ("Fail to read the header from the file (fread %s)", strerror (errno));
			stop = APPLICATION_QUIT;
		}

		/* Compute the difference between two time-stamps (only for test) */
		// timespec_substract (&result, &header.timestamp, &timespecOld[header.board]);

		/* Save the current time-stamp to compute the
		 * difference with the next one (only for test)
		 */
		// memcpy (&timespecOld[header.board], &(header.timestamp), sizeof (struct timespec));

		/* Get the number of channels in the sequence for the current board */
	    nChannels = header.config.board[header.board].nChannels;

		/* Compute the number of values (measured values)
		 * contained in the raw file
		 */
	    if (header.size <=0)
		{
			loginfo_error ("Size of data in the raw file is wrong");
			stop = APPLICATION_QUIT;
		}
		nData = header.size / sizeof (uint16_t);

		/* Compute the number of values per channels for the current board */
		nDataPerChannel = (nData / nChannels);

		/* For each channel an array of float is created.
		 * Compute the size of these arrays.
		 */
		size = (nDataPerChannel * sizeof (float));

		/* Create the directory in which to save the
		 * *.dat files.
		 */
		if (mkdir (header.config.log_file_path, 0777) < 0)
		{
			switch (errno)
			{
				case EEXIST:
				break;

				default:
					loginfo_error ("Fail to create %s directory (mkdir %s)", header.config.log_file_path, strerror (errno));
					stop = APPLICATION_QUIT;
			}
		}

		if (create_timestamp_file (header))
			goto error;
		/*nullify the variables for statistic*/
		set_statistic_to_zero(over_sum,average,deviation,variance,ep, 8);
		for (i=0; i<nChannels; i++)
		{
			/* Create a *.dat file for each channel */
			sprintf (dstfname, "%s/Channel_%u_%u_%u.dat", header.config.log_file_path, header.config.board[header.board].id[i], (uint32_t)header.timestamp.tv_sec, (uint32_t)header.timestamp.tv_nsec);
			fddat[i] = fopen (dstfname, "w+");
			if (fddat[i] < 0)
			{
				loginfo_error ("Fail to create %s file (open %s)", dstfname, strerror (errno));
				goto error;
			}

			/* Allocate memory to make computations on the data for each channel */
			fData[i] = (float *) realloc (fData[i], size);
			if (fData[i] == NULL)
			{
				loginfo_error ("Fail to allocate memory to make data computations (realloc %s)", strerror (errno));
				goto error;
			}
		}

		/* Allocate memory for the whole data */
		data = (uint16_t *) realloc (data, header.size);
		if (data == NULL)
		{
			loginfo_error ("Fail to allocate memory to make data computations (realloc %s)", strerror (errno));
			goto error;
		}

		/* Read all data in one time from the raw file */
		ret = fread (data, header.size, 1, fdraw);

		if ((ret < 0) || (ret != 1))
		{
			loginfo_error ("Fail to read data from the file (fread %s)", strerror (errno));
			stop = APPLICATION_QUIT;
		}

		fclose (fdraw);
		fdraw = NULL;

		channelDataIndex = 0;
		channel = 0;

		/* Make conversions and computations on data for each channels */
		for (i=0; (i<nData) && stop; i++)
		{
			/* Increment the value index in the channel array */
			if (channel == (nChannels - 1))
				channelDataIndex++;

			/* Increment the channel number */
			channel = (i % nChannels);

			/* Convert in volt and compute with coefficients */
			compute (header.config.board[header.board].polarity[channel],
					 header.config.board[header.board].gain[channel],
					 data[i],
					 header.config.board[header.board].coeff1[channel],
					 header.config.board[header.board].coeff2[channel],
					 &(fData[channel][channelDataIndex]));
			
			/*gather statistic*/
			over_sum[channel]+=(double) fData[channel][channelDataIndex];
		}
		for (i=0; i<nChannels && stop; i++)
		{
			average[i] = over_sum[i] / (double) nDataPerChannel;
		}
		/*compute statistic aacording to "NUMERICAL RECIPES, THIRD EDITION"*/
		channelDataIndex = 0;
		channel = 0;
		for (i=0; (i<nData) && stop; i++)
		{
			// Increment the value index in the channel array
			if (channel == (nChannels - 1))
				channelDataIndex++;

			// Increment the channel number 
			channel = (i % nChannels);

			temp_double = ((double) fData[channel][channelDataIndex])-average[channel];
                        deviation[channel] += fabs(temp_double);
                        ep[channel] += temp_double;
                        variance[channel] += temp_double*temp_double;
		}
		
		for (i=0; i<nChannels && stop; i++)
		{
			deviation[i] /= (double) nDataPerChannel;
	                variance[i]=(variance[i]-ep[i]*ep[i]/nDataPerChannel)/((double) nDataPerChannel - 1.0);
		}

		/* (only for test) */
		// loginfo_info ("Board %hu %010li sec. %010li nsec. (file: %s) %010li", header.board, result.tv_sec, result.tv_nsec, srcfname);
		loginfo_info ("Board %hu (file: %s) channels %d data per channel %d", header.board, srcfname,nChannels,nDataPerChannel);
		/*log info statistic*/
		for (i=0; i<nChannels && stop; i++)
		{
			if(header.config.board[header.board].is_on[i] == 1)
			{
				loginfo_info ("Board %hu (file: %s) channel_id %d (%s); average=%12.9e; deviation=%12.9e; variance=%12.9e;", header.board, srcfname,  header.config.board[header.board].id[i], header.config.board[header.board].label[i],average[i],deviation[i],variance[i]);
			}
		}
		/* Save data in a file */
		for (i=0; i<nChannels; i++)
		{
			ret = fwrite (fData[i], (nDataPerChannel * sizeof (float)), 1, fddat[i]);

			if ((ret < 0) || (ret != 1))
			{
				loginfo_error ("Fail to write data in the file (fwrite %s)", strerror (errno));
				goto error;
			}

			if (fData[i])
			{
				free (fData[i]);
				fData[i] = NULL;
			}

	    	fclose (fddat[i]);
	    	fddat[i] = NULL;
		}

		/* Delete the *.raw data once the *.dat is generated */
    	if (remove (srcfname) < 0)
		{
			loginfo_error ("Fail to remove file %s (remove %s)", srcfname, strerror (errno));
			goto error;
		}
	}

error:

	for (i=0; i<nChannels; i++)
	{
		if (fData[i])
			free (fData[i]);

		if (fddat[i])
			fclose(fddat[i]);
	}

	if (fdraw)
		fclose (fdraw);

	if (data)
		free (data);

	closelog ();

	return 0;
}

//------------------------------------------------------------------------------
