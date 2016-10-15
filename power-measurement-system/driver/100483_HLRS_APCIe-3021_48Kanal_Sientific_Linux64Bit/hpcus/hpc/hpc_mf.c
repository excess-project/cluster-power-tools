/**
 * hpc_mf.c - Format recorded measurement data and send it to the
 * monitoring frameworj ATOM
 *
 * This is a free Software.
 * Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
 *
 * Source code was created by
 * the High Performance Computing Center Stuttgart (HLRS)
 * University of Stuttgart
 * Nobelstraße 19
 * 70569 Stuttgart
 * Germany
 * Bugs to:khabi@hlrs.de
 **/



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


#include "hpc_config.h"
#include "hpc_signals.h"
#include "hpc_utils.h"
#include "hpc.h"
#include "revision.h"
#include "hpc_statistic.h"
#include "hpc_mf.h"
#include "hpc_curl.h"
#include "hpc_timespec.h"
 /* compute dc power for the monitoring framework*/
int hpc_mf_calculate_power( float** fData, int nChannels,int* fData_channel_idx, uint32_t filter_step_left,uint32_t filter_step_right ,struct fileHeader_t* header, struct iniPowerConfig_t* power_config, struct mf_messages_t* messages)
{
    uint32_t component;
    uint32_t nPowerComponents;
    uint32_t measure_interval_micro_sec;
    uint32_t average_interval_micro_sec;
    uint32_t current_channel_id;
    uint32_t voltage_channel_id=0;
    uint32_t measure_in_interval;
    uint32_t num_intervals;
    uint32_t nDataPerChannel;
    struct timespec current_time_timestamp;
    struct timespec start_of_sequence;
    double start_delta_micro_sec;
    double coeff1;
    double coeff2;
    double watt, volt_avg, current_avg, voltage_const,last_watt;
    long double sec_start, sec_end;
    double** watts;
    double *voltage_filtered, *current_filtered;
    struct timespec** start_measurement;
    struct timespec** end_measurement;
    int nData = 0;
    int ret = 0;
    int one_volt_curr_channel=0;

    nData = header->size / sizeof (uint16_t);
	  nDataPerChannel = (nData / nChannels);
    nPowerComponents = power_config->nPowerComponents;
    uint32_t ii, jj;
    if(nPowerComponents	> 0 )
    {
        measure_interval_micro_sec = header->config.acquisition_time*header->config.nChannels;
        start_delta_micro_sec = measure_interval_micro_sec*nDataPerChannel;
        if( timespec_substract_micro_seconds (&start_of_sequence,&header->timestamp, start_delta_micro_sec)<0 )
        {
            loginfo_error ("Timestamp for the measure sequency could not be obtained");
            goto error;
        }
        
        watts = (double**) malloc(nPowerComponents*sizeof(double*));
        start_measurement = (struct timespec**) malloc(nPowerComponents*sizeof(struct timespec*));
        end_measurement = (struct timespec**) malloc(nPowerComponents*sizeof(struct timespec*));
        messages->mf_messages = (char***)malloc(nPowerComponents*sizeof(char**));
        messages->num_intervals = (uint32_t*)malloc(nPowerComponents*sizeof(uint32_t));
        messages->num_components = nPowerComponents;
        voltage_filtered = (double*)malloc(nDataPerChannel*sizeof(double));
        current_filtered = (double*)malloc(nDataPerChannel*sizeof(double));
        for (component=0; (component<nPowerComponents); component++)
        {
           #ifdef __DO_INFO_SYSLOG_
           loginfo_info ("calculate power for component:%d", component);
           #endif
           current_time_timestamp.tv_sec = start_of_sequence.tv_sec;
           current_time_timestamp.tv_nsec = start_of_sequence.tv_nsec;
           average_interval_micro_sec = power_config->components[component].measure_interval_micro_sec;
           current_channel_id = fData_channel_idx[power_config->components[component].current_channel_id];//-first_channel_index;
/*         if(power_config->components[component].voltage_channel_id < 1000000)
           {
              voltage_channel_id = fData_channel_idx[power_config->components[component].voltage_channel_id];//-first_channel_index;
           }else
           {
              voltage_channel_id = power_config->components[component].voltage_channel_id;//-first_channel_index;
           }
*/
           if(power_config->components[component].voltage_channel_id == power_config->components[component].current_channel_id )
           {
              one_volt_curr_channel=1;
              voltage_channel_id=-1;
           }else
           {
              voltage_channel_id = fData_channel_idx[power_config->components[component].voltage_channel_id];
              one_volt_curr_channel=0;
           }
           coeff1 = (double) power_config->components[component].coeff1;
           coeff2 = (double) power_config->components[component].coeff2;
           measure_in_interval = average_interval_micro_sec / measure_interval_micro_sec;
           num_intervals = nDataPerChannel / measure_in_interval;
           watts[component] = (double*) malloc(num_intervals*sizeof(double));
           start_measurement[component] = (struct timespec*) malloc(num_intervals*sizeof(struct timespec));
           end_measurement[component] = (struct timespec*) malloc(num_intervals*sizeof(struct timespec));
           /*Apply median filter*/
           #ifdef __DO_INFO_SYSLOG_
           loginfo_info ("voltage_channel_id:%d", voltage_channel_id);
           loginfo_info ("current_channel_id:%d", current_channel_id);
           loginfo_info ("measure_interval_micro_sec:%d", measure_interval_micro_sec);
           loginfo_info ("average_interval_micro_sec:%d", measure_interval_micro_sec);
           loginfo_info ("measure_in_interval:%d", measure_in_interval);
           loginfo_info ("num_intervals:%d", num_intervals);
           loginfo_info ("Apply median filter for component:%d", component);
           #endif
           calculate_filter_no(current_filtered,fData[current_channel_id],nDataPerChannel, filter_step_left, filter_step_right);
           if(one_volt_curr_channel)
           {
               voltage_const = coeff1;
               for(ii=0; ii<nDataPerChannel; ii++)
               {
                   voltage_filtered[ii]=voltage_const;
               }
           }else
           {
               calculate_filter_no(voltage_filtered,fData[voltage_channel_id],nDataPerChannel, filter_step_left, filter_step_right);
           }
         /*  for(ii=0; ii<nDataPerChannel; ii++)
           {
               voltage_filtered[ii]=1.0;
               //current_filtered[ii]=fData[current_channel_id][ii];
           }*/
           #ifdef __DO_INFO_SYSLOG_
           loginfo_info ("current_channel_id: %i; voltage_channel_id: %i", current_channel_id,voltage_channel_id);
           loginfo_info ("average_interval_micro_sec: %i; measure_interval_micro_sec: %i; measure_in_interval: %i; num_intervals: %i", average_interval_micro_sec,measure_interval_micro_sec,measure_in_interval,num_intervals);
           #endif
           last_watt=0.0;
           for(ii=0; ii<num_intervals; ii++)
           {   
               watt = 0.0;
               volt_avg=0.0;
               current_avg=0.0;
               start_measurement[component][ii].tv_sec = current_time_timestamp.tv_sec;
               start_measurement[component][ii].tv_nsec = current_time_timestamp.tv_nsec;

               for(jj=ii*measure_in_interval; jj < (ii+1)*measure_in_interval; jj++)
               {
                   volt_avg =  voltage_filtered[jj];
                   current_avg = current_filtered[jj];
                   watt = watt + volt_avg*current_avg;
                   //watt = watt + volt_avg;
                   /*if(ii==0)
                   {
                       #ifdef __DO_INFO_SYSLOG_
                       loginfo_info ("current_channel_id:%d; voltage_channel_id:%d; interval: %i; jj: %d; watt: %12.6e\n",current_channel_id,voltage_channel_id,ii,jj, watt);
                       loginfo_info ("voltage: %12.6e; current: %12.6e\n", voltage_filtered[jj],current_filtered[jj]);
                       #endif
                   }*/
               }
               watt /= (double)(measure_in_interval);
               if(watt>SUPREMUM_WATT) watt=last_watt;
               if(one_volt_curr_channel!=1)
                  watt *= coeff1;
               watt += coeff2;
               watts[component][ii] = watt;
               if( timespec_add_micro_seconds(&current_time_timestamp,&current_time_timestamp,average_interval_micro_sec)<0 )
               {
                    loginfo_error ("Current Timestamp %i could not be obtained", ii);
                    ret = 1;
                    goto error;
               }
               end_measurement[component][ii].tv_sec = current_time_timestamp.tv_sec;
               end_measurement[component][ii].tv_nsec = current_time_timestamp.tv_nsec;
               #ifdef __DO_INFO_SYSLOG_
               loginfo_info ("watts[component][jj]: %f",watts[component][ii]);
               #endif
           }
           messages->mf_messages[component] = (char**)malloc(num_intervals*sizeof(char*));
           messages->num_intervals[component]=num_intervals;
           for(ii=0; ii<num_intervals; ii++)
           { 
               messages->mf_messages[component][ii] = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));
               sec_start = (long double)start_measurement[component][ii].tv_sec+\
                (long double)(start_measurement[component][ii].tv_nsec) / (long double)1.0e9;
                sec_end= (long double)end_measurement[component][ii].tv_sec+\
                (long double)(end_measurement[component][ii].tv_nsec) / (long double)1.0e9;
                /**
                * //sprintf(messages->mf_messages[component][ii],"{\"Timestamp\":\"%.9Lf\",\"Timestamp2\":\"%.9Lf\",\"%s\":\"%.6f\"}",\
                * //sec_start, sec_end,power_config->components[component].label,watts[component][ii]);
               **/
               sprintf(messages->mf_messages[component][ii],"{\"Timestamp\":\"%.9Lf\",\"type\":\"power\",\"%s\":%.6f}",\
               sec_start, power_config->components[component].label,watts[component][ii]);
           }
        }
        for (component=0; (component<nPowerComponents)&& stop; component++)
        {
            free(watts[component]);
            free(start_measurement[component]);
            free(end_measurement[component]);
        }
        free(watts);
        free(start_measurement);
        free(end_measurement);
        free(voltage_filtered);
        free(current_filtered);
    }
    
    error:
        
    return ret;
}

 /* compute ac power for the monitoring framework*/
int hpc_mf_calculate_power_ac( float** fData, int nChannels,struct fileHeader_t* header, struct iniPowerConfig_t* power_config, struct mf_messages_t* messages)
{
    uint32_t component;
    uint32_t nPowerComponents;
    uint32_t measure_interval_micro_sec;
    uint32_t average_interval_micro_sec;
    uint32_t watt_channel_id;
    uint32_t measure_in_interval;
    uint32_t num_intervals;
    uint32_t nDataPerChannel;
    struct timespec current_time_timestamp;
    struct timespec start_of_sequence;
    double start_delta_micro_sec;
    double coeff1;
    double coeff2;
    double watt,last_watt;
    long double sec_start, sec_end;
    double** watts;
    struct timespec** start_measurement;
    struct timespec** end_measurement;
    int nData = 0;
    int ret = 0;


    nData = header->size / sizeof (uint16_t);
	nDataPerChannel = (nData / nChannels);
    nPowerComponents = power_config->nPowerComponents;
    uint32_t ii, jj;
    if(nPowerComponents	> 0 )
    {
        loginfo_info ("hpc_mf_calculate_power_ac nPowerComponents: %i;",nPowerComponents );
        measure_interval_micro_sec = header->config.acquisition_time*header->config.nChannels;
        start_delta_micro_sec = measure_interval_micro_sec*nDataPerChannel;
        if( timespec_substract_micro_seconds (&start_of_sequence,&header->timestamp, start_delta_micro_sec)<0 )
        {
            loginfo_error ("Timestamp for the measure sequency could not be obtained");
            goto error;
        }
        
        watts = (double**) malloc(nPowerComponents*sizeof(double*));
        start_measurement = (struct timespec**) malloc(nPowerComponents*sizeof(struct timespec*));
        end_measurement = (struct timespec**) malloc(nPowerComponents*sizeof(struct timespec*));
        messages->mf_messages = (char***)malloc(nPowerComponents*sizeof(char**));
        messages->num_intervals = (uint32_t*)malloc(nPowerComponents*sizeof(uint32_t));
        messages->num_components = nPowerComponents;
        for (component=0; (component<nPowerComponents); component++)
        {
           loginfo_info ("hpc_mf_calculate_power_ac component: %i;",component );   
           current_time_timestamp.tv_sec = start_of_sequence.tv_sec;
           current_time_timestamp.tv_nsec = start_of_sequence.tv_nsec;
           average_interval_micro_sec = power_config->components[component].measure_interval_micro_sec;
           watt_channel_id = component;
           coeff1 = (double) power_config->components[component].coeff1;
           coeff2 = (double) power_config->components[component].coeff2;
           measure_in_interval = average_interval_micro_sec / measure_interval_micro_sec;
           num_intervals = nDataPerChannel / measure_in_interval;
           watts[component] = (double*) malloc(num_intervals*sizeof(double));
           start_measurement[component] = (struct timespec*) malloc(num_intervals*sizeof(struct timespec));
           end_measurement[component] = (struct timespec*) malloc(num_intervals*sizeof(struct timespec));
           
           #ifdef __DO_INFO_SYSLOG_
           loginfo_info ("watt_channel_id: %i;",watt_channel_id );
           loginfo_info ("average_interval_micro_sec: %i; measure_interval_micro_sec: %i; measure_in_interval: %i; num_intervals: %i", average_interval_micro_sec,measure_interval_micro_sec,measure_in_interval,num_intervals);
           #endif
           last_watt=0.0;
           for(ii=0; ii<num_intervals; ii++)
           {   
               watt = 0.0;
               start_measurement[component][ii].tv_sec = current_time_timestamp.tv_sec;
               start_measurement[component][ii].tv_nsec = current_time_timestamp.tv_nsec;

               for(jj=ii*measure_in_interval; jj < (ii+1)*measure_in_interval; jj++)
               {
                   /*if(ii==0)
                     {
                       #ifdef __DO_INFO_SYSLOG_
                       loginfo_info ("current_channel_id:%d; voltage_channel_id:%d; interval: %i; jj: %d; watt: %12.6e\n",current_channel_id,voltage_channel_id,ii,jj, watt);
                       loginfo_info ("current: %12.6e; voltage: %12.6e\n", fData[current_channel_id][jj],fData[voltage_channel_id][jj]);
                       #endif
                     }*/ 
                   watt = watt + (double)fData[component][jj];
               }
               watt /= (double)(measure_in_interval);
               watt *= coeff1;
               watt += coeff2;
               if(watt>SUPREMUM_WATT) watt=last_watt;
               watts[component][ii] = watt;
               if( timespec_add_micro_seconds(&current_time_timestamp,&current_time_timestamp,average_interval_micro_sec)<0 )
               {
                    loginfo_error ("Current Timestamp %i could not be obtained", ii);
                    ret = 1;
                    goto error;
               }
               end_measurement[component][ii].tv_sec = current_time_timestamp.tv_sec;
               end_measurement[component][ii].tv_nsec = current_time_timestamp.tv_nsec;
               #ifdef __DO_INFO_SYSLOG_
               loginfo_info ("watts[component][jj]: %f",watts[component][ii]);
               #endif
           }
           messages->mf_messages[component] = (char**)malloc(num_intervals*sizeof(char*));
           messages->num_intervals[component]=num_intervals;
           for(ii=0; ii<num_intervals; ii++)
           { 
               messages->mf_messages[component][ii] = (char*)malloc(MAX_MESSAGE_LENGTH*sizeof(char));
               sec_start = (long double)start_measurement[component][ii].tv_sec+\
                (long double)(start_measurement[component][ii].tv_nsec) / (long double)1.0e9;
                sec_end= (long double)end_measurement[component][ii].tv_sec+\
                (long double)(end_measurement[component][ii].tv_nsec) / (long double)1.0e9;
                /**
                * //sprintf(messages->mf_messages[component][ii],"{\"Timestamp\":\"%.9Lf\",\"Timestamp2\":\"%.9Lf\",\"%s\":\"%.6f\"}",\
                * //sec_start, sec_end,power_config->components[component].label,watts[component][ii]);
               **/
               sprintf(messages->mf_messages[component][ii],"{\"Timestamp\":\"%.9Lf\",\"type\":\"power_ac\",\"%s\":%.6f}",\
               sec_start, power_config->components[component].label,watts[component][ii]);
           }
        }
        for (component=0; (component<nPowerComponents)&& stop; component++)
        {
            free(watts[component]);
            free(start_measurement[component]);
            free(end_measurement[component]);
        }
        free(watts);
        free(start_measurement);
        free(end_measurement);
    }
    
    error:
        
    return ret;
}


int hpc_mf_free_messages(struct mf_messages_t* messages)
{
    uint32_t ii,jj;
    int ret=0;
    
    for(ii=0; ii<messages->num_components; ii++)
    {
         for(jj=0; jj<messages->num_intervals[ii]; jj++)
         {
             free(messages->mf_messages[ii][jj]);
         }
         free(messages->mf_messages[ii]);
     }
     free(messages->mf_messages);
     messages->mf_messages = NULL;
     
     return ret;
}


int hpc_mf_print_messages(struct mf_messages_t* messages)
{
    uint32_t ii,jj;
    int ret=0;
    
    for(ii=0; ii<messages->num_components; ii++)
    {
         for(jj=0; jj<messages->num_intervals[ii]; jj++)
         {
             loginfo_info("%s",messages->mf_messages[ii][jj]);
         }
        
     }
     
     return ret;
}

int hpc_mf_send_messages(struct curl_slist **headers_, CURL **curl_,char* mf_url, struct mf_messages_t* messages)
{
    uint32_t ii,jj;
    int ret=0;
    
    for(ii=0; ii<messages->num_components; ii++)
    {
         for(jj=0; jj<messages->num_intervals[ii]; jj++)
         {
             if(hpc_curl_send_monitoring_data(headers_,curl_,mf_url,messages->mf_messages[ii][jj])<0 )
             {
                loginfo_error ("hpc_mf_send_messages error component:%i; interval: %i;",ii,jj);
             }else
             {
                 // loginfo_info("%s is send",messages->mf_messages[ii][jj]);
             }

         }
     }
     
     return ret;
}

int hpc_mf_send_messages_all_in_one(struct curl_slist **headers_, CURL **curl_,char* mf_url, struct mf_messages_t* messages)
{
    uint32_t ii,jj;
    int ret=0;
    char* large_message;
    size_t offset;
    
    for(ii=0; ii<messages->num_components; ii++)
    {
        large_message = (char*)malloc(messages->num_intervals[ii]*MAX_MESSAGE_LENGTH*sizeof(char));
        sprintf(large_message,"{");
        offset = 1;
        
        for(jj=0; jj<messages->num_intervals[ii]; jj++)
        {
            sprintf(large_message+offset,"%s",messages->mf_messages[ii][jj]);
            offset = offset + strlen(messages->mf_messages[ii][jj]);
            if( jj==messages->num_intervals[ii]-1)
            {
                sprintf(large_message+offset,"}");
            }else
            {
                sprintf(large_message+offset,",");
            }
            offset = offset + 1;
        }
      //  fprintf(stderr,"%s",large_message);
        if(hpc_curl_send_monitoring_data(headers_,curl_,mf_url,large_message)<0 )
        {
            loginfo_error ("hpc_mf_send_messages_all_in_one error component:%i;",ii);
        }else
        {
           // loginfo_info("%s is send",messages->mf_messages[ii][jj]);
        }
        hpc_curl_finish(headers_,curl_);
        free(large_message);
    }

     return ret;
}

void hpc_mf_stop_messages(struct curl_slist **headers_, CURL **curl_)
{
    hpc_curl_finish(headers_,curl_);
}

