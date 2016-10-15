/*     
* File:   hpc_statictic.h
* Calculation of some statistical values
* The High Performance Computing Center Stuttgart (HLRS) of the University of Stuttgart
* Autor: Dmitry Khabi(email: khabi@hlrs.de)
* Created on Jun 5, 2014
*/

#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "hpc_utils.h"
#include "hpc_statistic.h"
/*Compute statistical values according to the "NUMERICAL RECIPES, THIRD EDITION"
* help procedure - see below for the details
*/
void calculate_statistical_values_per_channel(float* fData,double over_sum, int nDataPerChannel,double* average,double* adeviation,double* sdeviation, double* variance)
{
	int j;
	double temp_double;
	double temp_adev;
	double temp_var;
	double temp_avrg;
	double temp_ep;

	temp_adev = 0.0;
	temp_ep = 0.0;
	temp_var = 0.0;
	temp_avrg = over_sum / (double) nDataPerChannel;


	for (j=0; j<nDataPerChannel; j++)
	{
		temp_double = ((double) fData[j])-temp_avrg;
	        temp_adev += fabs(temp_double);
	        temp_ep += temp_double;
	        temp_var += temp_double*temp_double;
	}
	
	*adeviation= temp_adev/(double) nDataPerChannel;
  *variance=(temp_var-temp_ep*temp_ep/nDataPerChannel)/((double) nDataPerChannel - 1.0);
	*sdeviation= sqrt(*variance);
	*average=temp_avrg;
}
void calculate_filter_mean(double* outfData,float* infData, uint32_t rawdata_length, uint32_t step_left,uint32_t step_right)
{
    calculate_filter_median(outfData,infData, rawdata_length,step_left,step_right);
}
void calculate_filter_no(double* outfData,float* infData, uint32_t rawdata_length, uint32_t step_left,uint32_t step_right)
{
    uint32_t ii;
    for(ii=1;ii<rawdata_length;ii++)
    {
      outfData[ii] = (double)infData[ii];
    }
}
/*applying of mean filter on roh channel data
*out outfData - filtered values1-dim array, must be allocated)
*in infData - roh values (1-dim array)
*in rawdata_length - length of infData
*in step_left - patch size: left half-window size
*in step_right - patch size: left half-window size
*/
void calculate_filter_median(double* outfData,float* infData, uint32_t rawdata_length, uint32_t step_left,uint32_t step_right)
{
  uint32_t ii, jj, length;
  double average, sum_right, sum_left, sum;
 
  for(ii=0;ii<step_left;ii++)
  {
    sum_right = 0.0;
    sum_left = 0.0;
    length = 1;
    for(jj=0;jj<ii;jj++)
    {
      sum_left = sum_left + (double)infData[jj];
      length = length+ 1;
    }
    for(jj=ii+1;jj<ii+step_right+1;jj++)
    {
      sum_right = sum_right + (double)infData[jj];
      length = length+ 1;
    }
    sum = sum_right + sum_left + (double)infData[ii];
    average = sum / (double)length;
    outfData[ii] = average;
  }
 
  for(ii=step_left; ii<rawdata_length-step_right; ii++)
  {
    sum_right = 0.0;
    sum_left = 0.0;
    length = 1;
    for(jj=ii-step_left; jj<ii; jj++)
    {
     sum_left = sum_left + (double)infData[jj];
     length = length+ 1;
    }
    for(jj=ii+1;jj<ii+step_right+1; jj++)
    {
     sum_right = sum_right + (double)infData[jj];
     length = length+ 1;
    }
    sum = sum_right + sum_left +(double)infData[ii];
    average = sum /(double) length;
    outfData[ii] = average;
  }
 
  for(ii=rawdata_length-step_right;ii<rawdata_length;ii++)
  {
    sum_right = 0.0;
    sum_left = 0.0;
    length = 1;
    for(jj=ii-step_left;jj<ii;jj++)
    {
      sum_left = sum_left + (double)infData[jj];
      length = length+ 1;
    }
    for(jj=ii+1;jj<rawdata_length;jj++)
    {
      sum_right = sum_right + (double)infData[jj];
      length = length+ 1;
    }
    sum = sum_right + sum_left + (double)infData[ii];
    average = sum /(double) length;
    outfData[ii] = average;
  }
       loginfo_info ("Apply median filter end");
}
 

/*Compute statistic according to the "NUMERICAL RECIPES, THIRD EDITION"
*in fData - measured values (2-dim array)
*in over_sum - sum over the measured values per channel
*in nChannels - number of channels
*in nDataPerChannel - number of measuremenets per channel
*out average - mean average per channel (1-dim array)
*out adeviation - average deviation per channel (1-dim array)
*out adeviation - average deviation per channel (1-dim array)
*out adeviation - standard deviation per channel (1-dim array)
*out variance - variance per channel (1-dim array)
*/
void calculate_statistical_values(float** fData, double* over_sum,int nChannels,int nDataPerChannel,double* average, double* adeviation, double* sdeviation, double* variance)
{
	int i;
	for (i=0; i<nChannels; i++)
	{
		calculate_statistical_values_per_channel(fData[i],over_sum[i],nDataPerChannel,average+i,adeviation+i,sdeviation+i,variance+i);
	}
}

