#ifndef io_profile_h
#define io_profile_h
#define FORMAT_SCIENTIFIC "%2.12e"
#define FORMAT_LONG "%12lu"
#include <stdio.h>
struct profile_results
{
  long long* seconds;
  long long* nanoseconds;
};
struct performance_results
{
  long long* base_freq;
  long long* gpu_freq;
  long long* cpu_freq;
  unsigned long long* tsc;
  double* time_sec;
  long long* wbytes;
  long long* rbytes;
  long long* host_gpu_bytes;
  long long* gpu_host_bytes;
  long long* flops;
  long long* size;
  long long* num_tests;
  unsigned long long* omp_atomics;
  unsigned long long* omp_barriers;
  unsigned long long* num_threads;
};
extern struct profile_results start_time;
extern struct profile_results end_time;
void init_profile_timestamps(int profiles_num, struct profile_results** start_phase, struct profile_results** end_phase );
void free_profile_timestamps(int profiles_num, struct profile_results* start_phase, struct profile_results* end_phase );
void init_profile_performance(int profiles_num, struct performance_results** performance_results );
void free_profile_performance(struct performance_results* perf_results );
void write_out_profile(long first_profile_idx, long num_profiles, FILE* file_ptr, struct profile_results* start_phase, struct profile_results* end_phase, int verbose);
void write_out_performance(long first_profile_idx, long num_profiles, FILE* file_ptr, struct performance_results* perf_results, int verbose);
FILE* open_profile_file(long experiment_id, char *top_dir_path, const char *file_name);
FILE* open_performance_file(long experiment_id, char *top_dir_path, const char *file_name);
#endif
