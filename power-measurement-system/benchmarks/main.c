/* 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include "io_profile.h"
#include "timer_profile.h"
#include "affinity.h"
#include "cpufreq_user.h"
#include "inout_parameters.h"
#include "dot_product_omp_atomic.h"

#define FIRST_FREQ 0
#define LAST_FREQ 14
#define FIRST_CORES 1
#define LAST_CORES CPUFREQ_NUMBER_OF_CORES/2
#define LOOP_REPETITIONS 10
#define VECTOR_LENGTH_MIN 1024
#define VECTOR_LENGTH_MAX 42949664
#define VECTOR_LENGTH_MULT_STEP 2
#define STR_MAX 128

int main(int argc, char*argv[]) 
{
    char opt;
    /* Set default conf. values*/
    int job_id = 0; //PBS_JOBID
    char out_dir[512]; //Directory to save a profile file
    char benchmark_name[STR_MAX]; //Name of the benchmark
    int num_repetitions=LOOP_REPETITIONS; //Number of the repetitions of the benchmark test
    int num_min_cores=FIRST_CORES;
    int num_max_cores=LAST_CORES;
    int idx_min_freq=FIRST_FREQ;
    int idx_max_freq=LAST_FREQ;
    int num_phases; //all possible configuration of the benchmark (number of cores and freq)
    unsigned long* avail_freq, *correct_avail_freq;
    long number_of_cpufrequencies;
    long long vector_length_min=VECTOR_LENGTH_MIN;
    long long vector_length_max=VECTOR_LENGTH_MAX;
    long long vector_length_mult_step=VECTOR_LENGTH_MULT_STEP;
    long long vector_length;
    int verbosity=0;
    int ii, jj, kk, curr_phase;
    double* __restrict aa;
    double* __restrict bb;
    int ff, err;
    void *temp_alloc;
    long long start_local_sec, start_local_nanosec;//temporally variables for the time stamp capture
    long long end_local_sec, end_local_nanosec;//temporally variables for the time stamp capture
    struct profile_results *start_time, *end_time;//arrays to save the time stamps for each computational phase
    struct performance_results* perf_results;//arrays with perf results
    struct output_parameters* func_output = (struct output_parameters*)malloc(1*sizeof(struct output_parameters));
    struct input_parameters* func_input;
    sprintf(out_dir,"./");
    sprintf(benchmark_name,"usleep");
        /* Read command line parameters */
    while( (opt = getopt(argc, argv, "j:o:t:n:e:b:r:k:l:p:x:s:v:h")) != -1 ) {
        switch(opt) {
            case 'j':
                job_id = atol(optarg);
                break;
            case 'o':
                sprintf(out_dir,"%s",optarg);
                break;
            case 't':
                sprintf(benchmark_name,"%s",optarg);
                break;
            case 'n':
                num_repetitions = atol(optarg);
                break;
            case 'e':
                num_min_cores = atol(optarg);
                break;
            case 'b':
                num_max_cores = atol(optarg);
                break;
            case 'r':
                idx_min_freq = atol(optarg);
                break;
            case 'k':
                idx_max_freq = atol(optarg);
                break;
            case 'l':
                vector_length_min = atol(optarg);
                break;
            case 'x':
                vector_length_max = atol(optarg);
                break;
            case 's':
                vector_length_mult_step = atol(optarg);
                break;
            case 'v':
                verbosity = atol(optarg);
                break;
            case 'h':
                printf("\t-j<int> Experiment id (e.x. PBS_JOBID);\n");
                printf("\t-t<char[128]> Benchmark name; (DOT_ATOMIC)\n");
                printf("\t-o<char[51]> Output directory for the profiling data with the timestamps;\n");
                printf("\t-n<int> Number of the repetitions for the selected benchmark;\n");
                printf("\t-e <int>\t: min number of threads (default: 1)\n");
                printf("\t-b <int>\t: max. number of threads (default: number of CPU cores)\n");
                printf("\t-r <int>\t:  min frequency index(default: 0)\n");
                printf("\t-k <int>\t:  max frequency index(default: max; see below)\n");
                printf("\t-l <int>\t:  min length of vector(default: %d)\n",VECTOR_LENGTH_MIN);
                printf("\t-x <int>\t:  max length of vector(default: %d)\n",VECTOR_LENGTH_MAX);
                printf("\t-s <int>\t:  mult. step for length of vector per thread(default: %d)\n",VECTOR_LENGTH_MULT_STEP);
                printf("\t-v<int> Verbosity level [0-silent mode];\n");
                printf("\t-h This help.\n");
                return 0;
                break;

            default:
                break;
        }
    }
    //print out the parameters (can be parsed by the post-processer)
    printf("\t-j %lu\n", job_id);
    printf("\t-t %s\n",  benchmark_name);
    printf("\t-o %s\n",  out_dir);
    printf("\t-n %lu\n", num_repetitions);
    printf("\t-e %lu\n", num_min_cores);
    printf("\t-b %lu\n", num_max_cores);
    printf("\t-r %lu\n",  idx_min_freq);
    printf("\t-k %lu\n", idx_max_freq);
    printf("\t-l %lu\n",  vector_length_min);
    printf("\t-x %lu\n", vector_length_max);
    printf("\t-s %lu\n", vector_length_mult_step);
    printf("\t-v %lu\n", verbosity);
    
    //number of various configurations in the experiment
    num_phases=0;
    for(ii=num_min_cores; ii<=num_max_cores;ii++)
      for(jj=idx_min_freq; jj<=idx_max_freq;jj++)
        for(kk=vector_length_min; kk<vector_length_max+1;kk*=vector_length_mult_step)
          num_phases++;
        
    //init the arrays with the time stamps for power profiling
    init_profile_timestamps(num_phases, &start_time, &end_time);
    //inti the arrays with the performance info
    init_profile_performance(num_phases, &perf_results );
    curr_phase=0;
    //create and pin threads
    #pragma omp parallel num_threads(num_max_cores)
    {
       affinity_set_aff();
    }
    //Init frequency setter
    avail_freq = Nullptr(unsigned long);
    err = 0;
    err = cpufreq_set_userspace_governor_policy(CPUFREQ_NUMBER_OF_CORES,&avail_freq, &number_of_cpufrequencies);
    correct_avail_freq = cpufreq_get_correct_cpufreq(avail_freq);
    if(verbosity>1)
    {
	   for (ii=0; ii<number_of_cpufrequencies;ii++)
		  printf("frequency(%d):%zu, correct_frequnecy:%zu;\n",(int)ii,avail_freq[ii],correct_avail_freq[ii]);
    }

    if (err != 0)
        fprintf(stderr, "userspace governor was set(?) with errors (root rights - sudo?) \n");
    if (idx_max_freq < 0 || idx_max_freq >= number_of_cpufrequencies)
        idx_max_freq = number_of_cpufrequencies - 1;
    if (idx_min_freq < 0 || idx_min_freq >= number_of_cpufrequencies)
        idx_min_freq = 0;
  
    curr_phase = 0;
    //START EXPERIMENT
    /*CHANGE CORES NUM LOOP */
    for(ii=num_min_cores; ii<=num_max_cores;ii++)
    {
        omp_set_num_threads(ii);
        /*CHANGE FREQ. LOOP */
        for(jj=idx_min_freq; jj<=idx_max_freq;jj++)
        {
            for(ff=0; ff<CPUFREQ_NUMBER_OF_CORES; ff++)
            {
              err = cpufreq_set_userspace_cpufrequency(ff,avail_freq[number_of_cpufrequencies - jj - 1]);
              if (verbosity > 1)
                  fprintf(stderr, "power stream->set cpu frequency (now %lu); tried to set: %lu (core: %d) \n", cpufreq_current_frequency(ff), avail_freq[number_of_cpufrequencies - jj - 1], ff);
            }
            /*CHANGE LENGTH LOOP*/
            for(kk=vector_length_min; kk<vector_length_max+1;kk*=vector_length_mult_step)
            {
                vector_length = kk*ii;
                //allocate and init memory - OK for one socket
                posix_memalign(&temp_alloc, (size_t)32, sizeof(double)*vector_length);
                aa=(double*)temp_alloc;
                posix_memalign(&temp_alloc, (size_t)32, sizeof(double)*vector_length);
                bb=(double*)temp_alloc;
                #pragma omp parallel num_threads(ii) shared(aa,bb,ii,jj,kk)
                {
                    double* __restrict local_a;
                    double* __restrict local_b;
                    int thread_num = omp_get_thread_num();
                    local_a = aa+thread_num*kk;
                    local_b = bb+thread_num*kk;
                    long long ll;
                    //make first touch
                    for(ll=0;ll<kk;ll++)
                    {
                        local_a[ii]=(double)ii;
                        local_b[ii]=((double)ii)*2.0;
                    }
                }
                //TEST OMP ATOMIC DOTPRODUCT
                #pragma omp parallel num_threads(ii) shared(number_of_cpufrequencies,perf_results,benchmark_name,aa,bb,ii,jj,kk,num_repetitions,curr_phase,func_output,start_local_sec,start_local_nanosec,end_local_sec,end_local_nanosec) private(func_input)
                {
                    long long ll;
                    double* __restrict local_a;
                    double* __restrict local_b;
                    func_input = (struct input_parameters*)malloc(1*sizeof(struct input_parameters));
                    int thread_num = omp_get_thread_num();
                    local_a = aa+thread_num*kk;
                    local_b = bb+thread_num*kk;
                    func_input->aa=local_a;
                    func_input->bb=local_b;
                    func_input->length=kk;
                    func_input->num_repetitions=num_repetitions;
                    if (strcmp(benchmark_name, "DOT_ATOMIC") == 0) 
                    {
                        //START PROFILING
                        #pragma omp barrier
                        #pragma omp master
                        timer_get_time_stamp(&start_local_sec,&start_local_nanosec);
                        //START BENCHMARK
                        dot_product_omp_atomic(func_input,func_output);
                        //END PROFILING
                        #pragma omp barrier
                        #pragma omp master
                        {
                            timer_get_time_stamp(&end_local_sec,&end_local_nanosec);
                            //SAVE RESULTS (start seconds, nanoseconds and end seconds, nanoseconds)
                            start_time->seconds[curr_phase]=start_local_sec;
                            start_time->nanoseconds[curr_phase]=start_local_nanosec;
                            end_time->seconds[curr_phase]=end_local_sec;
                            end_time->nanoseconds[curr_phase]=end_local_nanosec;
                            perf_results->num_tests[curr_phase]=func_input->num_repetitions;
                            perf_results->size[curr_phase]=func_input->length;
                            perf_results->wbytes[curr_phase]=func_input->length*sizeof(double);
                            perf_results->rbytes[curr_phase]=2*func_input->length*sizeof(double);
                            perf_results->flops[curr_phase]=1*func_input->length;
                            perf_results->omp_atomics[curr_phase]=1*func_input->length;
                            perf_results->omp_barriers[curr_phase]=0;
                            perf_results->time_sec[curr_phase]=(double)(end_local_sec-start_local_sec)+(double)(end_local_nanosec-start_local_nanosec)*1.0e-9;
                            perf_results->num_threads[curr_phase]=ii;
                            perf_results->cpu_freq[curr_phase]=avail_freq[number_of_cpufrequencies - jj - 1];
                            curr_phase++;
                        }
                        
                    } 
                    else /* default: */
                    {
                         fprintf(stderr,"Error: Wrong benchmark name!!\n");
                    }
                    free(func_input);
                } //OMP PARALLEL TEST
                free(aa);
                free(bb);
            } //LENGTH LOOP
        } //FREQ LOOP
    }//CORES NUM LOOP

    //WRITE OUT THE RESULTS
    char profilefilename[512];
    char performancefilename[512];
    sprintf(profilefilename, "%i-%s-profile.dat",job_id,benchmark_name);
    sprintf(performancefilename, "%i-%s-performance.dat",job_id,benchmark_name);
    FILE* file_profile=open_profile_file(job_id, out_dir, profilefilename);
    if(file_profile <= (FILE*) 0)
    {
        fprintf(stderr,"Error: Couldn't write out the profile!!\n");
    }else
    {
        write_out_profile(0,num_phases,file_profile, start_time, end_time, verbosity);
        fclose(file_profile);
    }
    FILE* file_peformance=open_performance_file(job_id, out_dir, performancefilename);
    if(file_profile <= (FILE*) 0)
    {
        fprintf(stderr,"Error: Couldn't write out the performance!!\n");
    }else
    {
        write_out_performance(0,num_phases, file_peformance, perf_results, verbosity);
        fclose(file_peformance);
    }
    free_profile_timestamps(num_phases, start_time, end_time );
    free_profile_performance(perf_results );
    free(func_output);
    return 0;
}

