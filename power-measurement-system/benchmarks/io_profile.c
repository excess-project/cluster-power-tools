#include "io_profile.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

void _write_out_profile_header(FILE* file_ptr);
void _write_out_performance_header(FILE* file_ptr);
//Inizialized the profile_results structure for the record of the start and the end time of the phases in the application
//int profiles_num (IN) - Max. number of the phases
//int struct profile_results** start_phase (INOUT) - pointer to the structure with the arrays to save the start time of the pahses
//int struct profile_results** end_phase (INOUT) -  pointer to the structure with the arrays
void init_profile_timestamps(int profiles_num, struct profile_results** start_phase, struct profile_results** end_phase )
{
    int ii;
    *start_phase = (struct profile_results*) malloc(sizeof(struct profile_results));
    *end_phase = (struct profile_results*) malloc(sizeof(struct profile_results));
    (*start_phase)->seconds = (long long*) malloc(sizeof(long long)*profiles_num);
    (*start_phase)->nanoseconds = (long long*) malloc(sizeof(long long)*profiles_num);
    (*end_phase)->seconds = (long long*) malloc(sizeof(long long)*profiles_num);
    (*end_phase)->nanoseconds = (long long*) malloc(sizeof(long long)*profiles_num);
    for(ii=0; ii<profiles_num; ii++)
    {
        (*start_phase)->seconds[ii] = 0;
        (*end_phase)->nanoseconds[ii] = 0; 
    }
}

void free_profile_timestamps(int profiles_num, struct profile_results* start_phase, struct profile_results* end_phase )
{
    int ii;
    free((start_phase)->seconds);
    free((start_phase)->nanoseconds);
    free((end_phase)->seconds); 
    free((end_phase)->nanoseconds); 
    free(start_phase);
    free(end_phase);
}


void init_profile_performance(int profiles_num, struct performance_results** perf_results )
{
  int ii; 
  *perf_results = (struct performance_results*) malloc(sizeof(struct performance_results));
  (*perf_results)->tsc=(unsigned long long*)malloc(sizeof(unsigned long long)*profiles_num);
  (*perf_results)->time_sec=(double*)malloc(sizeof(double)*profiles_num);
  (*perf_results)->wbytes=(long long*)malloc(sizeof(long long)*profiles_num);
  (*perf_results)->rbytes=(long long*)malloc(sizeof(long long)*profiles_num);
  (*perf_results)->host_gpu_bytes=(long long*)malloc(sizeof(long long)*profiles_num);
  (*perf_results)->gpu_host_bytes=(long long*)malloc(sizeof(long long)*profiles_num);
  (*perf_results)->flops=(long long*)malloc(sizeof(long long)*profiles_num);
  (*perf_results)->base_freq=(long long*)malloc(sizeof(long long)*profiles_num);
  (*perf_results)->gpu_freq=(long long*)malloc(sizeof(long long)*profiles_num);
  (*perf_results)->cpu_freq=(long long*)malloc(sizeof(long long)*profiles_num);
  (*perf_results)->size=(long long*)malloc(sizeof(long long)*profiles_num);
  (*perf_results)->num_tests=(long long*)malloc(sizeof(long long)*profiles_num);
  (*perf_results)->omp_atomics=(long long*)malloc(sizeof(long long)*profiles_num);
  (*perf_results)->omp_barriers=(long long*)malloc(sizeof(long long)*profiles_num);
  (*perf_results)->num_threads=(long long*)malloc(sizeof(long long)*profiles_num);

  for(ii=0; ii<profiles_num; ii++)
  {
    (*perf_results)->tsc[ii]=0;
    (*perf_results)->time_sec[ii]=0.0;
    (*perf_results)->wbytes[ii]=0;
    (*perf_results)->rbytes[ii]=0;
    (*perf_results)->host_gpu_bytes[ii]=0;
    (*perf_results)->gpu_host_bytes[ii]=0;
    (*perf_results)->flops[ii]=0;
    (*perf_results)->base_freq[ii]=0;
    (*perf_results)->gpu_freq[ii]=0;
    (*perf_results)->cpu_freq[ii]=0;
    (*perf_results)->size[ii]=0;
    (*perf_results)->num_tests[ii]=0;
    (*perf_results)->omp_atomics[ii]=0;
    (*perf_results)->omp_barriers[ii]=0;
    (*perf_results)->num_threads[ii]=0;
  }
}

void free_profile_performance(struct performance_results* perf_results )
{
    free(perf_results->tsc);
    free(perf_results->time_sec);
    free(perf_results->wbytes);
    free(perf_results->rbytes);
    free(perf_results->host_gpu_bytes);
    free(perf_results->gpu_host_bytes);
    free(perf_results->flops);
    free(perf_results->base_freq);
    free(perf_results->gpu_freq);
    free(perf_results->cpu_freq);
    free(perf_results->size);
    free(perf_results->num_tests);
    free(perf_results->omp_atomics);
    free(perf_results->omp_barriers);
    free(perf_results->num_threads);
    free(perf_results);
}
//Open and get the file pointer in order to write out in this file the start and end phases of the application;
//The old file with the same name will be replaced;
//long experiment_id (IN) - id of the experiment;
//char* top_dir_path (IN) - name of the top directory for the profile files;
//char* file_name (IN) - name of the the profile file;
//FILE* (RETURN) - pointer to the profile file; if 0 - error: could not create the file;
FILE* open_profile_file(long experiment_id, char *top_dir_path, const char *file_name)
{
  char* dir_name;
  char* file_path;
  FILE* file_profile_ptr;
  dir_name = (char*) malloc(sizeof (char)*256);
  sprintf(dir_name, "%s/%lu", top_dir_path,experiment_id);
  mkdir(dir_name, S_ISUID + S_ISGID + S_ISVTX + S_IRUSR + S_IWUSR + S_IXUSR + S_IRGRP + S_IWGRP + S_IXGRP + S_IROTH + S_IWOTH + S_IXOTH);
  file_path = (char*) malloc(1024 * sizeof (char));
  sprintf(file_path, "%s/%s", dir_name,file_name);
  unlink(file_path);
  file_profile_ptr = fopen(file_path, "w");
  if (file_profile_ptr == (FILE*)0) {
      fprintf(stderr, "error in profiling: could not create the file %s in the directory %s\n", file_name, dir_name);
  }
  free(dir_name);
  free(file_path);
  return file_profile_ptr;
}

//Open and get the file pointer in order to write out in this file the performance statistic of the application;
//The old file with the same name will be replaced;
//long experiment_id (IN) - id of the experiment;
//char* top_dir_path (IN) - name of the top directory for the profile files;
//char* file_name (IN) - name of the the performance file;
//FILE* (RETURN) - pointer to the performance file; if 0 - error: could not create the file;
FILE* open_performance_file(long experiment_id, char *top_dir_path, const char *file_name)
{
  char* dir_name;
  char* file_path;
  FILE* file_perfromance_ptr;
  dir_name = (char*) malloc(sizeof (char)*256);
  sprintf(dir_name, "%s/%lu", top_dir_path,experiment_id);
  mkdir(dir_name, S_ISUID + S_ISGID + S_ISVTX + S_IRUSR + S_IWUSR + S_IXUSR + S_IRGRP + S_IWGRP + S_IXGRP + S_IROTH + S_IWOTH + S_IXOTH);
  file_path = (char*) malloc(1024 * sizeof (char));
  sprintf(file_path, "%s/%s", dir_name,file_name);
  unlink(file_path);
  file_perfromance_ptr = fopen(file_path, "w");
  if (file_perfromance_ptr == (FILE*)0) {
      fprintf(stderr, "error in profiling: could not create the file %s in the directory %s\n", file_name, dir_name);
  }
  free(dir_name);
  free(file_path);
  return file_perfromance_ptr;
}



//Write out the profile files with the start and the end of the phases in the application;
//long first_profile_idx (IN) - Write out the phases beginning with the first_profile_idx
//long num_profiles (IN) - Total number of phases
//FILE* file_ptr (IN) - Pointer to the formated file, which was previously opened with the standard function "open_profile_file";
//struct profile_results* start_phase (IN) - Array with the Timestamps for the start of the phases in the application;
//struct profile_results* end_phase (IN) - Array with the Timestamps for the end of the phases in the application;
//int verbose - if >0 some debug output is switched on
void write_out_profile(long first_profile_idx, long num_profiles, FILE* file_ptr, struct profile_results* start_phase, struct profile_results* end_phase, int verbose)
{
    size_t ii;
    _write_out_profile_header(file_ptr);
    for (ii = first_profile_idx; ii < num_profiles; ii++) 
    {
        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t) ii);
        if(verbose>0)
          fprintf(stdout, "idx:"FORMAT_LONG" ", (size_t) ii);

        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t) start_phase->seconds[ii]);
        if(verbose>0)
          fprintf(stdout, "start_phase_s:"FORMAT_LONG" ", (size_t) start_phase->seconds[ii]);

        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t) start_phase->nanoseconds[ii]);
        if(verbose>0)
          fprintf(stdout, "start_phase_ns:"FORMAT_LONG" ", (size_t) start_phase->nanoseconds[ii]);

        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t) end_phase->seconds[ii]);
        if(verbose>0)
          fprintf(stdout, "end_phase_s:"FORMAT_LONG" ", (size_t) end_phase->seconds[ii]);

        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t) end_phase->nanoseconds[ii]);
        if(verbose>0)
          fprintf(stdout, "end_phase_ns:"FORMAT_LONG" ", (size_t) end_phase->nanoseconds[ii]);

        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, "\n");
        if(verbose>0)
          fprintf(stdout, "\n");
    }

}

//Write out the header for the profile files with the start and end phases of the application;
//FILE* file_ptr (IN) - Pointer to the formated file, which was previously opened with the standard function "open_profile_file";
void _write_out_profile_header(FILE* file_ptr) {
    if (file_ptr != (FILE*)0)
    {
        fprintf(file_ptr, "id;");
        fprintf(file_ptr, "start sec;");
        fprintf(file_ptr, "start nanosec;");
	      fprintf(file_ptr, "end_phase sec;");
        fprintf(file_ptr, "end_phase nanosec;");
        fprintf(file_ptr, "\n");
   }
}

void _write_out_performance_header(FILE* file_ptr) {
    if (file_ptr != (FILE*)0)
    {
        fprintf(file_ptr, "idx;");
        fprintf(file_ptr, "base_freq;");
        fprintf(file_ptr, "cpu_freq;");
        fprintf(file_ptr, "gpu_freq;");
        fprintf(file_ptr, "tsc;");
        fprintf(file_ptr, "time_sec;");
        fprintf(file_ptr, "wbytes;");
        fprintf(file_ptr, "rbytes;");
        fprintf(file_ptr, "host_gpu_bytes;");
        fprintf(file_ptr, "gpu_host_bytes;");
        fprintf(file_ptr, "size;");
        fprintf(file_ptr, "num_tests;");
	    fprintf(file_ptr, "flops;");
        fprintf(file_ptr, "omp_atomics;");
        fprintf(file_ptr, "omp_barriers;");
        fprintf(file_ptr, "num_threads;");
        fprintf(file_ptr, "\n");
   
   }
}
void write_out_performance(long first_profile_idx, long num_profiles, FILE* file_ptr, struct performance_results* perf_results, int verbose)
{
    size_t ii;
    _write_out_performance_header(file_ptr);
    for (ii = first_profile_idx; ii < num_profiles; ii++) 
    {
        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t) ii);
        if(verbose>0)
          fprintf(stdout, "idx:"FORMAT_LONG" ", (size_t) ii);

        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t) perf_results->base_freq[ii]);
        if(verbose>0)
          fprintf(stdout, "base_freq:"FORMAT_LONG" ", (size_t)  perf_results->base_freq[ii]);
         if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t) perf_results->cpu_freq[ii]);
        if(verbose>0)
          fprintf(stdout, "cpu_freq:"FORMAT_LONG" ", (size_t)  perf_results->cpu_freq[ii]);

        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t) perf_results->gpu_freq[ii]);
        if(verbose>0)
          fprintf(stdout, "gpu_freq:"FORMAT_LONG" ", (size_t)  perf_results->gpu_freq[ii]);

        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t)  perf_results->tsc[ii]);
        if(verbose>0)
          fprintf(stdout, "tsc:"FORMAT_LONG" ", (size_t)  perf_results->tsc[ii]);

        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_SCIENTIFIC";",  perf_results->time_sec[ii]);
        if(verbose>0)
          fprintf(stdout, "time_sec:"FORMAT_SCIENTIFIC" ",   perf_results->time_sec[ii]);

        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t) perf_results->wbytes[ii]);
        if(verbose>0)
          fprintf(stdout, "wbytes:"FORMAT_LONG" ", (size_t) perf_results->wbytes[ii]);

        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t) perf_results->rbytes[ii]);
        if(verbose>0)
          fprintf(stdout, "rbytes:"FORMAT_LONG" ", (size_t) perf_results->rbytes[ii]);

        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t) perf_results->host_gpu_bytes[ii]);
        if(verbose>0)
          fprintf(stdout, "host_gpu_bytes:"FORMAT_LONG" ", (size_t) perf_results->host_gpu_bytes[ii]);

        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t) perf_results->gpu_host_bytes[ii]);
        if(verbose>0)
          fprintf(stdout, "gpu_host_bytes:"FORMAT_LONG" ", (size_t) perf_results->gpu_host_bytes[ii]);

        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t) perf_results->size[ii]);
        if(verbose>0)
          fprintf(stdout, "size:"FORMAT_LONG" ", (size_t) perf_results->size[ii]);

        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t) perf_results->num_tests[ii]);
        if(verbose>0)
          fprintf(stdout, "num_tests:"FORMAT_LONG" ", (size_t) perf_results->num_tests[ii]);



        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t) perf_results->flops[ii]);
        if(verbose>0)
          fprintf(stdout, "flops:"FORMAT_LONG" ", (size_t) perf_results->flops[ii]);

        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t) perf_results->omp_atomics[ii]);
        if(verbose>0)
          fprintf(stdout, "omp_atomics:"FORMAT_LONG" ", (size_t) perf_results->omp_atomics[ii]);
        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t) perf_results->omp_barriers[ii]);
        if(verbose>0)
          fprintf(stdout, "omp_barriers:"FORMAT_LONG" ", (size_t) perf_results->omp_barriers[ii]);
        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, FORMAT_LONG";", (size_t) perf_results->num_threads[ii]);
        if(verbose>0)
          fprintf(stdout, "num_threads:"FORMAT_LONG" ", (size_t) perf_results->num_threads[ii]);
    
        if (file_ptr != (FILE*)0)
            fprintf(file_ptr, "\n");
        if(verbose>0)
          fprintf(stdout, "\n");
    }

}
