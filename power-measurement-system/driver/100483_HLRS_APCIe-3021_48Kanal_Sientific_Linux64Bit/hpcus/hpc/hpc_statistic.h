/*     
* File:   hpc_statictic.h
* Calculation of some statistical values
* The High Performance Computing Center Stuttgart (HLRS) of the University of Stuttgart
* Autor: Dmitry Khabi(email: khabi@hlrs.de)
* Created on Jun 5, 2014
*/

#ifndef CALC_STATICTIC_H_
#define CALC_STATICTIC_H_
/*Compute statistic according to the "NUMERICAL RECIPES, THIRD EDITION"*/
void calculate_statistical_values(float** fData, double* over_sum,int nChannels,int nDataPerChannel,double* average, double* adeviation, double* sdeviation, double* variance);
void calculate_filter_median(double* outfData,float* infData, uint32_t rawdata_length, uint32_t step_left,uint32_t step_right);
void calculate_filter_mean(double* outfData,float* infData, uint32_t rawdata_length, uint32_t step_left,uint32_t step_right);
void calculate_filter_no(double* outfData,float* infData, uint32_t rawdata_length, uint32_t step_left,uint32_t step_right);
#endif
