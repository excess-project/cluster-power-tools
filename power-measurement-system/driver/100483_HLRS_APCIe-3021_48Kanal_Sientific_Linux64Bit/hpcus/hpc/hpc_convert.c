/**
 * hpc_convert.c - read raw data of hpc_measure, converts it into
 * the unis (A,V,W), write the results in new files and
 * sends the results to the monitoring framework ATOM through
 * curl if needed. The coefficients and relation of the measured channel 
 * are defined in the configuration files. 
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
#include <curl/curl.h>
#include <pthread.h>
#include "hpc.h"
#include "hpc_config.h"
#include "hpc_signals.h"
#include "hpc_utils.h"
#include "revision.h"
#include "hpc_statistic.h"
#include "hpc_mf.h"
#include "hpc_timespec.h"
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

/**  Print main options
 *
 * 
 * @retval void: 
 */
void print_options_help()
{
    printf("\t-r <string>\t:,Directory, which contains the files with raw data;\n") ;
    printf("\t-c <string>\t: Path to the file, which contains the description and and measure configuration of the components (i.e. CPU). The address of the monitoring framework ATOM is also deined in the file;\n");
    printf("\t-d <string>\t: String with db key to send the converted data to the monitoring framewotk ATOM; If empty or equals to \"0\", the data wouldn't be sent;\n");
    printf("\t-b <int>\t: Median filter - left window half-size (if d != \"0\");\n");
    printf("\t-f <int>\t: Median filter - right window half-size (if d != \"0\");\n");
    printf("\t-s <int>\t: If >0 ,save the converted data to the files;\n");
    printf("\t-h : show this help text\n");
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
		loginfo_error ("Fail to get files list in %s (scandir %s)",dir, strerror (errno));
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
    FILE *fddat[NUMBER_OF_CHANNELS_ON_BOARD] = {NULL};
    FILE *fdraw = NULL;
    int fData_channel_idx[NUMBER_OF_BOARDS*NUMBER_OF_CHANNELS_ON_BOARD]= {-1};
    char dstfname[256] = {0};
    char srcfname[256] = {0};
    char buf[256] = {0};
    char mf_url[1024] = {0};
    struct fileHeader_t header;
    int nChannels = 0;
    int channel = 0;
    int size = 0;
    int channelDataIndex = 0;
    int nData = 0;
    float *fData[NUMBER_OF_CHANNELS_ON_BOARD] = {NULL};
    uint16_t * data = NULL;
    int nDataPerChannel = 0;
    int i = 0;
    int nfiles = 0;
    struct iniPowerConfig_t power_config;
    int connectMF=0;
    struct curl_slist *headers_ = NULL;
    char opt,config_file[1024], directory_path[1024];
    char dbkey[256] = {0};
    int filter_step_left;
    int filter_step_right;
    CURL *curl_ =NULL;
    int stop_local;
    int save_to_disk=0;
    char compute_node[256];
    char username[256];
    char jobname[256];
    int param_t;

    filter_step_left=10;
    filter_step_right=10;

    /* Check the command line parameters
    * There should be the path in which raw files
    * are saved, path to config file with the power components and dbkey to send the data to the Monitoring Framework (0- don't send anything)
    */

    if (argc < 4)
    {
    print_options_help();
    return EXIT_FAILURE;
    } 
    while ((opt = (char) (getopt(argc, argv, "r:c:d:b:f:s:h:t:u:p:n:?"))) != -1) {
    switch (opt) {
       case 'c': sprintf(config_file, "%s", optarg);break;
       case 'r': sprintf(directory_path, "%s", optarg);break;
       case 'p': sprintf(compute_node, "%s", optarg);break;
       case 'u': sprintf(username, "%s", optarg);break;
       case 'n': sprintf(jobname, "%s", optarg);break;
       case 'd': sprintf(dbkey,"%s", optarg);break;
       case 't': param_t = atoi(optarg);break;
       case 'b': filter_step_left = atoi(optarg);break;
       case 'f': filter_step_right = atoi(optarg);break;
       case 's': save_to_disk = atoi(optarg);break;
       case 'h':
       case '?':
                print_options_help();
                return 0;
    }
    }

    #ifdef __DO_INFO_SYSLOG_
    double over_sum[NUMBER_OF_CHANNELS_ON_BOARD]={0.0};
    double average[NUMBER_OF_CHANNELS_ON_BOARD]={0.0};
    double adeviation[NUMBER_OF_CHANNELS_ON_BOARD]={0.0};
    double sdeviation[NUMBER_OF_CHANNELS_ON_BOARD]={0.0};
    double variance[NUMBER_OF_CHANNELS_ON_BOARD]={0.0};
    #endif
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


        
    
    /*read power confiruration of cluster components*/
    power_config.components=(struct components_t*)0;
    if ( hpc_read_power_config_file (config_file, &power_config) < 0 )
    goto error;

    if(dbkey[0]=='0' && dbkey[1]==(char)0) 
    {
       connectMF = 0;
    }
    else
    {
       connectMF = 1;
       sprintf(mf_url,"%s/%s",power_config.mf_url,dbkey);
    }
    loginfo_info("connect to mf (0-no, 1-yes): %i; dbkey:%s;",connectMF,dbkey);
    loginfo_info("left and right window half-size : %i; :%i;",filter_step_left,filter_step_right);
    pthread_mutex_lock(&stop_mutex);
    stop_local=stop;
    pthread_mutex_unlock(&stop_mutex);
    while (stop_local == APPLICATION_RUN || nfiles > 0)
    {
        /* Get the number, and names, of raw files to convert.
         * The oldest is always converted in first.
         */
        nfiles = get_raw_file_name (directory_path, buf);

        /* No files to convert, loop again */
        if (nfiles == 0)
        {
            pthread_mutex_lock(&stop_mutex);
            stop_local=stop;
            pthread_mutex_unlock(&stop_mutex);
            continue;
        }
        /* Can't get files, error */
        if (nfiles < 0)
            goto error;

        /* Open the raw file */
        sprintf (srcfname, "%s/%s", directory_path, buf);
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
            goto error;
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
            pthread_mutex_lock(&stop_mutex);
            stop = APPLICATION_QUIT;
            pthread_mutex_unlock(&stop_mutex);
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
                    pthread_mutex_lock(&stop_mutex);
                    stop = APPLICATION_QUIT;
                    pthread_mutex_unlock(&stop_mutex);
            }
        }
        if(save_to_disk==1)
        {
          if (create_timestamp_file (header))
            goto error;
        }

        for (i=0; i<nChannels; i++)
        {
            /* Allocate memory to make computations on the data for each channel */
            fData[i] = (float *) realloc (fData[i], size);
            fData_channel_idx[header.config.board[header.board].id[i]]=i;
            #ifdef __DO_INFO_SYSLOG_
            loginfo_info ("channel_id: %i; idx_id: %i", header.config.board[header.board].id[i],i);
            #endif
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
            pthread_mutex_lock(&stop_mutex);
            stop = APPLICATION_QUIT;
            pthread_mutex_unlock(&stop_mutex);
        }

        fclose (fdraw);
        fdraw = NULL;

        channelDataIndex = 0;
        channel = 0;
        #ifdef __DO_INFO_SYSLOG_
        for (i=0;i<NUMBER_OF_CHANNELS_ON_BOARD;i++) over_sum[i]=0.0;
        #endif
        /* Make conversions and computations on data for each channels */
        for (i=0; (i<nData) ; i++)
        {
            /* Increment the value index in the channel array */
            if (channel == (nChannels - 1))
                channelDataIndex++;

            /* Increment the channel number */
            channel = (i % nChannels);

            /* Convert in volt or ampere and compute with coefficients */
            compute (header.config.board[header.board].polarity[channel],
                     header.config.board[header.board].gain[channel],
                     data[i],
                     header.config.board[header.board].coeff1[channel],
                     header.config.board[header.board].coeff2[channel],
                     &(fData[channel][channelDataIndex]));
            
            /*gather statistic*/
            #ifdef __DO_INFO_SYSLOG_
            over_sum[channel]+=(double) fData[channel][channelDataIndex];
            #endif
        }
        /* (only for test) */
        // loginfo_info ("Board %hu %010li sec. %010li nsec. (file: %s) %010li", header.board, result.tv_sec, result.tv_nsec, srcfname);
        #ifdef __DO_INFO_SYSLOG_
        loginfo_info ("Board %hu (file: %s -> timestamp: %u_%u) channels %d data per channel %d", header.board, srcfname, (uint32_t)header.timestamp.tv_sec, (uint32_t)header.timestamp.tv_nsec,nChannels,nDataPerChannel);
        /*compute statistical values*/
        calculate_statistical_values(fData,over_sum,nChannels,nDataPerChannel,average,adeviation,sdeviation,variance);
        for (i=0; i<nChannels ; i++)
        {
            loginfo_info ("%s: mean average=%12.6e; avrg. deviation=%12.6e; std.  deviation=%12.6e; variance=%12.6e;",header.config.board[header.board].label[i],average[i],adeviation[i],sdeviation[i],variance[i]);
        }
        #endif   
        loginfo_info ("Save data in a file from %s:; senden:%d", srcfname , connectMF);
        if(connectMF > 0 )
        {
            struct mf_messages_t messages;
            if (hpc_mf_calculate_power(fData, nChannels,fData_channel_idx,filter_step_left,filter_step_right,&header,&power_config, &messages )<0)
            {
              loginfo_error ("Fail to compose the mf messages");
                  goto error;
              connectMF = 0;
            }else
            {
              hpc_mf_send_messages(&headers_, &curl_,mf_url, &messages);
              #ifdef __DO_INFO_SYSLOG_
              hpc_mf_print_messages(&messages);
              #endif
              hpc_mf_free_messages(&messages);
            }
        }
        if(save_to_disk==1)
        {
          /* Save data in a file */
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
              ret = fwrite (fData[i], (nDataPerChannel * sizeof (float)), 1, fddat[i]);
              if ((ret < 0) || (ret != 1))
              {
                loginfo_error ("Fail to write data in the file (fwrite %s)", strerror (errno));
                goto error;
              }
              fclose (fddat[i]);
              fddat[i] = NULL;
          }
        }
        for (i=0; i<nChannels; i++)
        {
            if (fData[i])
            {
              free (fData[i]);
              fData[i] = NULL;
            }
        }
        /* Delete the *.raw data once the *.dat is generated */
        if (remove (srcfname) < 0)
        {
            loginfo_error ("Fail to remove file %s (remove %s)", srcfname, strerror (errno));
            goto error;
        }
        pthread_mutex_lock(&stop_mutex);
        stop_local=stop;
        pthread_mutex_unlock(&stop_mutex);
        if(stop_local !=APPLICATION_RUN)
        {
           nfiles = get_raw_file_name (directory_path, buf);
           #ifdef __DO_INFO_SYSLOG_
           loginfo_info ("get stop signal: %i fileas are stiil to convert; last timestamp: %i", nfiles,header.timestamp.tv_nsec);
           #endif
        }
    }

error:

    for (i=0; i<nChannels; i++)
    {
       if (fData[i])
          free (fData[i]);
       if(save_to_disk)
       {
         if (fddat[i])
           fclose(fddat[i]);
       }
    }

    if (fdraw)
    fclose (fdraw);

    if (data)
    free (data);

    hpc_mf_stop_messages(&headers_,&curl_);
    closelog ();
    pthread_mutex_lock(&stop_mutex);
    stop = APPLICATION_QUIT;
    pthread_mutex_unlock(&stop_mutex);
    pthread_mutex_destroy(&stop_mutex);
    return 0;
}

//------------------------------------------------------------------------------
