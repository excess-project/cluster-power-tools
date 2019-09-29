#ifndef cpufreq_user_h
#define cpufreq_user_h
#include <cpufreq.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <malloc.h>

#ifdef CPU_E5_2680V3
#undef CPU_E5_2687W
//#define CPU_E5_2680V3 
#undef CPU_E5_2690V2  
#define CPUFREQ_NUMBER_OF_CORES 48
#define CPU_BASE_FREQ 2.5004e9
#endif
#ifdef CPU_E5_2690V2
#undef CPU_E5_2687W
#undef CPU_E5_2680V3 
//#define CPU_E5_2690V2  
#define CPUFREQ_NUMBER_OF_CORES 40
#define CPU_BASE_FREQ 2.993e9
#endif
#ifdef CPU_E5_2687W
//#define CPU_E5_2687W
#undef CPU_E5_2680V3 
#undef CPU_E5_2690V2  
#define CPUFREQ_NUMBER_OF_CORES 16
#define CPU_BASE_FREQ 3.5004e9
#endif

#define CPUFREQ_NUMBER_OF_CORES_BY_PRINT 1
#define Nullptr(type) (type*)0
#define _PATH_PROC_CPUINFO	"/proc/cpuinfo"


unsigned long cpufreq_current_frequency(unsigned int cpu);
unsigned long* cpufreq_get_available_cpufrequencies(unsigned int cpu, long* number_of_cpufrequencies);
void cpufreq_print_available_cpufrequencies();
void cpufreq_print_available_governors();
int cpufreq_set_userspace_governor_policy(unsigned int cpu_id,unsigned long** cpufrequencies,  long* number_of_cpufrequencies);
int cpufreq_set_ondemand_governor_policy(unsigned int cpu_id);
int cpufreq_set_userspace_cpufrequency(unsigned int cpu_id ,unsigned long cpufrequency);
void cpufreq_print_copyright();
void cpufreq_print_options_help();
unsigned long* cpufreq_get_correct_cpufreq(unsigned long* available_cpufrequencies);
#endif

