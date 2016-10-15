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
#include <math.h>
#include <pthread.h>
#include "hpc_config.h"
#include "hpc_signals.h"
#include "hpc_utils.h"
#include "hpc.h"
#include "revision.h"
#include "hpc_timespec.h"
#include "hpc_statistic.h"
#include "hpc_mf.h"


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
    printf("\t-r <string>\t:,Directory, which contains the files with raw data\n") ;
//    printf("\t-c <string>\t: Path to the file, which contains the description of the measuredcomponents )\n");
    printf("\t-d <string>\t: String with db key to send the converted data to the MF);\n");
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


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int main (int argc, char** argv)
{
    int ret = 0;
    FILE *fddat[8] = {NULL};
    FILE *fdraw = NULL;
    FILE *fddat_pw[8] = {NULL};
    char mf_url[1024] = {0};
    char srcfname[256] = {0};
    char dstfname_pw[256] = {0};
    char buf[256] = {0};
    struct fileHeader_t header;
    int nChannels = 0;
    int channel = 0;
    int size = 0;
    int channelDataIndex = 0;
    int nData = 0;
    float *fData[8] = {NULL};
    float *fData_pw[8] = {NULL};
    uint16_t * data = NULL;
    int nDataPerChannel = 0;
    int ac_voltage_channel_id = 0;
    int ac_num_periods = 1;
    int ac_num_avrg_periods = 1;
    int ac_measures_in_avrg_periods = 1;
    int ac_channel_idx = 0;
    double measure_frequency;
    double temp_sum;
    int size_pw;
    int i = 0;
    int j = 0;
    int k = 0;
    int nfiles = 0;
    double over_sum[8];
    double average[8];
    double adeviation[8];
    double sdeviation[8];
    double variance[8];
    double max_converted[8];
    double min_converted[8];
    struct iniPowerConfig_t power_config;
    char opt, directory_path[1024],config_file[1024];
    char dbkey[256] = {0};
    int connectMF=0;
    struct curl_slist *headers_ = NULL;
    CURL *curl_ =NULL;
    int stop_local;
    int save_to_disk=0;
    // struct timespec result; // (only for test)
    // struct timespec timespecOld[4]; // (only for test)

	

    /* Check the command line parameters
    * There should be the path in which raw files
    * are saved, path to config file with the power components and dbkey to send the data to the Monitoring Framework (0- don't send anything)
    */

    if (argc < 3)
    {
        print_options_help();
        return EXIT_FAILURE;
    } 
    while ((opt = (char) (getopt(argc, argv, "r:s:d:c:h:?"))) != -1) {
      switch (opt) {
       case 'c': sprintf(config_file, "%s", optarg);break;
       case 'r': sprintf(directory_path, "%s", optarg);break;
       case 'd': sprintf(dbkey,"%s", optarg);break;
       case 's': save_to_disk = atoi(optarg);break;
       case 'h':
       case '?':
                print_options_help();
                return 0;
      }
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
  #ifdef __DO_INFO_SYSLOG_
  loginfo_info("connect to mf (0-no, 1-yes): %i; dbkey:%s;",connectMF,dbkey);
  loginfo_info("power_config->nPowerComponents: %i;",power_config.nPowerComponents);
  #endif
  pthread_mutex_lock(&stop_mutex);
  stop_local=stop;
  pthread_mutex_unlock(&stop_mutex);
  nfiles = 0;
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
        #ifdef __DO_INFO_SYSLOG_
        loginfo_info("srcfname : %s;",srcfname);
        #endif
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
            goto error;
		}
		nData = header.size / sizeof (uint16_t);
		/* Compute the number of values per channels for the current board */
		nDataPerChannel = (nData / nChannels);
        #ifdef __DO_INFO_SYSLOG_
        loginfo_info("nDataPerChannel : %i;",nDataPerChannel);
        #endif
		/* For each channel an array of float is created.
		 * Compute the size of these arrays.
		 */
		size = (nDataPerChannel * sizeof (float));
		/* check the channel for the current profile in ac circuit */
        
		ac_voltage_channel_id = header.config.ac_voltage_channel_id;
		if (ac_voltage_channel_id <=0 || ac_voltage_channel_id > 31)
		{
			loginfo_error ("Channel id for current profile in ac circuit is wrong");
            goto error;
		}
        ac_channel_idx = 4;		/*check the number of periods in ac circuit for the calculation of the power consumption
		* and calculate	ac_measures_in_avrg_periods
		*/
		ac_num_periods = header.config.ac_num_periods;
		measure_frequency = 1.0e6 / ( ((double)header.config.acquisition_time)*((double)nChannels) ) ;
		ac_measures_in_avrg_periods = ac_num_periods * ceil (measure_frequency / 50.0);
		ac_num_avrg_periods = floor (nDataPerChannel/ac_measures_in_avrg_periods);
		size_pw = size; //ac_num_avrg_periods *sizeof (float);
        #ifdef __DO_INFO_SYSLOG_
		loginfo_info ("ac_num_periods=%d; measure_frequency=%f; ac_measures_in_avrg_periods=%d; ac_num_avrg_periods=%d; size_pw=%d; nDataPerChannel=%d;nData=%d",ac_num_periods,measure_frequency,ac_measures_in_avrg_periods,ac_num_avrg_periods,size_pw,nDataPerChannel,nData);
		#endif
        if (ac_measures_in_avrg_periods <= ac_num_periods)
		{
			loginfo_error ("acquisition_time is too big");
            goto error;
		}
		if (ac_measures_in_avrg_periods > nDataPerChannel)
		{
			loginfo_error ("ac_num_periods is to big (check realtion between 50 Hz, ac_num_periods, acquisition_time and number_of_sequences_per_interrupt");
            goto error;
		}

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
			if (fData[i] == NULL)
			{
				loginfo_error ("Fail to allocate memory to make data computations (realloc %s)", strerror (errno));
				goto error;
			}
                        fData_pw[i] = (float *) realloc (fData_pw[i], size_pw);
               //loginfo_info ("power variables for channel %d are initialized",i);
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
    for (i=0;i<nChannels;i++)max_converted[i]=-999999.0;
    for (i=0;i<nChannels;i++)min_converted[i]=999999.0;
		/* Make conversions and computations on data for each channels */
		for (i=0; (i<nData); i++)
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
			if(max_converted[channel] < fData[channel][channelDataIndex]) max_converted[channel] = fData[channel][channelDataIndex];
			if(min_converted[channel] > fData[channel][channelDataIndex]) min_converted[channel] = fData[channel][channelDataIndex];
		}

		/*calculate power consumption*/
		for (i=0;i<8;i++) over_sum[i]=0.0;
		for(k=0; k<nChannels; k++)
		{
      for(i=0; i<ac_num_avrg_periods;i++)
      {
        temp_sum=0.0;
        for(j=0; j<ac_measures_in_avrg_periods;j++)
        {
         temp_sum = temp_sum + (((double)fData[k][j+i*ac_measures_in_avrg_periods])*((double)fData[ac_channel_idx][j+i*ac_measures_in_avrg_periods]));
        }
        temp_sum = (temp_sum/((double)ac_measures_in_avrg_periods));
        for(j=0; j<ac_measures_in_avrg_periods;j++)
         fData_pw[k][i*ac_measures_in_avrg_periods+j] = temp_sum;
        over_sum[k] += temp_sum;
      }
		}
		/*compute statistical values*/
		calculate_statistical_values(fData_pw,over_sum,nChannels,ac_num_avrg_periods,average,adeviation,sdeviation,variance);
		for (i=0; i<nChannels;i++)
		{
			//loginfo_info ("%s: max converted: %12.6e; min converted: %12.6e;",header.config.board[header.board].label[i],max_converted[i],min_converted[i]);
		loginfo_info ("%s: %12.6e Watt; mean average=%12.6e; avrg. deviation=%12.6e; std.  deviation=%12.6e; variance=%12.6e;",header.config.board[header.board].label[i],over_sum[i]/ac_num_avrg_periods,average[i],adeviation[i],sdeviation[i],variance[i]);
		}
        /* (only for test) */
		// loginfo_info ("Board %hu %010li sec. %010li nsec. (file: %s) %010li", header.board, result.tv_sec, result.tv_nsec, srcfname);
    #ifdef __DO_INFO_SYSLOG_
    loginfo_info ("Board %hu (file: %s -> timestamp: %u_%u) channels %d data per channel %d / %d", header.board, srcfname,(uint32_t)header.timestamp.tv_sec, (uint32_t)header.timestamp.tv_nsec,nChannels,nDataPerChannel,ac_num_avrg_periods);
    #endif

    #ifdef __DO_INFO_SYSLOG_
    loginfo_info ("Save data in a file from %s:; senden:%d", srcfname , connectMF);
    #endif
    if(connectMF > 0 )
    {
      struct mf_messages_t messages;
      if (hpc_mf_calculate_power_ac(fData_pw, nChannels, &header,&power_config, &messages )<0)
      {
        loginfo_error ("Fail to compose the mf messages");
        connectMF = 0;
        goto error;
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
        /* Create a *.dat file for the power consumption devices */
        sprintf (dstfname_pw, "%s/PChannel_%u_%u_%u.dat", header.config.log_file_path, header.config.board[header.board].id[i], (uint32_t)header.timestamp.tv_sec, (uint32_t)header.timestamp.tv_nsec);
        fddat_pw[i] = fopen (dstfname_pw, "w+");
        if (fddat_pw[i] < 0)
        {
          loginfo_error ("Fail to create %s file (open %s)", dstfname_pw, strerror (errno));
          goto error;
        }
        ret = fwrite (fData_pw[i],  (nDataPerChannel * sizeof (float)), 1, fddat_pw[i]); //_pw
        if ((ret < 0) || (ret != 1))
        {
          loginfo_error ("Fail to write data in the file (fwrite %s)", strerror (errno));
          goto error;
         }
         fclose (fddat_pw[i]);
         fddat_pw[i] = NULL;
      }
    }
    for (i=0; i<nChannels; i++)
    {
      if (fData[i])
      {
        free (fData[i]);
        fData[i] = NULL;
      }
	    if (fData_pw[i])
      {
	       free (fData_pw[i]);
	       fData_pw[i] = NULL;
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
    if(stop_local != APPLICATION_RUN)
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
           if(save_to_disk == 1)
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
