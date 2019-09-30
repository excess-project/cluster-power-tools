 
 ██████╗ ██████╗ ███╗   ██╗ ██████╗██╗   ██╗██████╗ ██████╗ ███████╗███╗   ██╗████████╗
██╔════╝██╔═══██╗████╗  ██║██╔════╝██║   ██║██╔══██╗██╔══██╗██╔════╝████╗  ██║╚══██╔══╝
██║     ██║   ██║██╔██╗ ██║██║     ██║   ██║██████╔╝██████╔╝█████╗  ██╔██╗ ██║   ██║   
██║     ██║   ██║██║╚██╗██║██║     ██║   ██║██╔══██╗██╔══██╗██╔══╝  ██║╚██╗██║   ██║   
╚██████╗╚██████╔╝██║ ╚████║╚██████╗╚██████╔╝██║  ██║██║  ██║███████╗██║ ╚████║   ██║   
 ╚═════╝ ╚═════╝ ╚═╝  ╚═══╝ ╚═════╝ ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═══╝   ╚═╝   
             ████████╗██╗  ██╗██████╗ ███████╗ █████╗ ██████╗ ███████╗                 
             ╚══██╔══╝██║  ██║██╔══██╗██╔════╝██╔══██╗██╔══██╗██╔════╝                 
                ██║   ███████║██████╔╝█████╗  ███████║██║  ██║███████╗                 
                ██║   ██╔══██║██╔══██╗██╔══╝  ██╔══██║██║  ██║╚════██║                 
                ██║   ██║  ██║██║  ██║███████╗██║  ██║██████╔╝███████║                 
                ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═════╝ ╚══════╝                 
                                                                                       
                  High Performance Computing Center Stuttgart (HLRS)
                             University of Stuttgart



The benchmark is intended to help to estimate the power consumption and performance of two kernels:
XGEMM and DOT product using  the pragma omp atomic. The size, range of active threads and CPU frequency
are configurable.
########################################################################################################
                              Preparing of application
########################################################################################################
The application main is divided in N phases.
In each phase the same procedure, dor or XGEMM, is called only the parameters, such as 
data size, cpu frequency or number of active threads are varied.
Before and after each of the phases the current timesteps of the system will be temporally saved. 
Hence, the timesteps indicate the start and stop of the benchmark phases.
At the end of the execution the application write out the profile file with the timestemps:
Here is an example of a profile file with N=2 (for example execution with 1 and 2 threads):
#id;start sec;start nanosec;;end_phase sec;end_phase nanosec;
 0;  1447163802;   321960833;  1447163802;   332036481;
 1;  1447163802;   332036885;  1447163802;   342098148;

##########################################################################################################
                              Compiling of the application
##########################################################################################################
The benchmark can be configured for both double- and single-precision floating-point numbers.
Please define the precision in the script for the compiling. The default setting is double.
The benchmark uses the library libcpufreq to change the frequency.
We support explicitly three CPU models:
CPU_E5_2687W - Sandy Bridge CPU 
CPU_E5_2690V2 - Ivy Bridge CPU
CPU_E5_2680V3 - Haswell CPU
The futher models can be easily added in the header file  cpufreq_user.h. The user needs to
define the maximal number of cores and the base frequency of CPU. 
The model of CPU must be also chosen during with an pre-processor directive, for example -DCPU_E5_2680V3.
The script can be found in the file ./comp.sh
##########################################################################################################
                                  Command line parameters
##########################################################################################################
To start the main application use the existing PBS script job. If necessary, adjust the environment 
varibales in the upper part of the script. The values of the env. variables will be  passed to the 
command line of the application (see section "Start the benchmark" of this document).
The application main has the following command line parameters:
$./main -h
-j <int>: Experiment id (e.x. PBS_JOBID);
-t <char[128]>: Benchmark name;
-o <char[51]>: Output directory for the profiling data with the timestamps;
-n <int>: Number of the repetitions for the selected benchmark;
-w <int>: Max. duration time in sec. for each test;
-i <int>: Duration of sleep in sec. between the tests;
-e <int>: Min number of threads;
-b <int>: Max. number of threads;
-r <int>: Min frequency index;
-k <int>: Max frequency index;
-l <int>: Min length of vector;
-x <int>: Max length of vector;
-s <int>: Mult. step for length of vector per thread;
-v <int>: Verbosity level [0-silent mode];
-h      : This help.\n");

############################################################################################################
                                  Preparing on the cluster environment
############################################################################################################
If you use the EXCESS cluster first time, please configure the power measurement with the following actions:
To start the power consumption of the nodes an user has to do the following:
--Create a config directory (replace 0X with the number of the node 01,02 or 03 to profile):
$mkdir ~/.pwm/node0X/ -p
--Switch on power record for each node of the cluster 
--(replace 0X with the number of the node to profile):
$touch ~/.pwm/node0X/copy_raw_data  
--Pack and save the power data in the directory ~/pwm/:
$touch ~/.pwm/node0X/save_raw_data  
--Send the measurement to the mf atom
$touch ~/.pwm/node0X/send_data_to_mf

Note: If you don't need the power data delete the file save_raw_data
$rm ~/.pwm/node0X/save_raw_data  

Note: If you don't want to send the data to the database of the mf
delete the file
$rm ~/.pwm/node0X/send_data_to_mf

Note: If you want to switch off the power measurement
delete the file
$rm ~/.pwm/node0X/copy_raw_data

############################################################################################################
                                              Start the benchmark
############################################################################################################

Please, replace the strings "USER" and "PROJECT_DIRPATH" to your username and the top directory in the 
following script and create two directories PROJECT_DIRPATH/messages and PROJECT_DIRPATH/data.

------------------------------------------------------------------------------------------------------------
#!/bin/sh
#PBS -l nodes=1:node03:ppn=20
#PBS -l walltime=00:10:00
#PBS -q batch
#PBS -M hpcdkhab@hlrs.de
#PBS -N PARALLEL_STORM_ATOMIC
#PBS -o PROJECT_DIRPATH/messages/${PBS_JOBID}.out
#PBS -e PROJECT_DIRPATH/messages/${PBS_JOBID}.err

PROJECT_DIR=PROJECT_DIRPATH

module load compiler/intel/14.0.2

NUM_JOBID=$(cut -d '.'  -f 1 <<< "$PBS_JOBID") #Extract numerical part of a PBS_JOBID
BENCH="DOT_ATOMIC"
OUT_DIR="${PROJECT_DIR}/data/"
NUM_REP=100
NUM_MIN_CORES=1
NUM_MAX_CORES=12
MIN_FREQ=0
MAX_FREQ=14
MIN_VECTOR_LENGTH=1024
MAX_VECTOR_LENGTH=32768
VECTOR_LENGTH_MULT_STEP=2
VERBOSITY=1 #Verbosity level 0 - silent mode
SLEEP_DURATION=2
MIN_TEST_DURATION=3

if [ ! -d "$OUT_DIR" ]; then
  mkdir $OUT_DIR -p
fi

${PROJECT_DIR}/main  -j $NUM_JOBID  -t $BENCH -o $OUT_DIR -n $NUM_REP -e $NUM_MIN_CORES -b $NUM_MAX_CORES\
                      -r $MIN_FREQ -k $MAX_FREQ -l $MIN_VECTOR_LENGTH -x $MAX_VECTOR_LENGTH -s \
                      $VECTOR_LENGTH_MULT_STEP -v $VERBOSITY -i SLEEP_DURATION -w MIN_TEST_DURATION


----------------------------------------------------------------------------------------------------------------

################################################################################################################
                                  Analyse the power consumption during the job
################################################################################################################

The tool "power_calculate" computes a power consumption of various components of the cluster.
The profile file will be used by the tool to calculate the power consumption for each phase.
The output of the tool are two files. The first file contains the power metrics for each phase.
The "ID" colum shows the number of the phase. The "Time" colum shows the duration of the phase.
The "num_measure" column shows how many power measurement were done during the phase.
The column "CPU1_node0X_Watt" shows what was the average power consumption of the first CPU on the node0X.
The columns "min.., max.." shows the max and min watts during the phase:
ID;Time;num_measure;CPU1_node0X_Watt;min_CPU1_node0X_Watt;max_CPU1_node0X_Watt;CPU1_node0X_Joule;....
There are also the values for CPU2, GPU and so on

#################################################################################################################
                                        The power analysis of the job
#################################################################################################################
The script ./analyse.sh is an example how to use the "power_calculate" tool and how to visualize the results. 
The visualization is done with the R-Language. The R Language script is saved in the directory ./r/

Call the script ./analyse.sh with the following parameters:
PBS_JOBID - numerical part of the PBS_JOBID (numbers before the first dot)
NODE - the compute node, which was used for the job execution (node01, node02 or node03)
BENCHMARK -  "XGEMM" or DOT_ATOMIC

Here is an example for the job 122294.fe 
$./analyse.sh 122294 node03 DOT_ATOMIC
In the directory data/122294/ you will find a generated pdf file and the cvs file with the power and performance 
metrics for each phase (configuration ) of the application.

Please see the example diagrams in the doc subdirectory.
####################################################################################################################
                                                  Acknowledgment
####################################################################################################################

This project is particularly realized through EXCESS. EXCESS is funded by the EU 7th Framework Programme 
                               (FP7/2013-2016) under grant agreement number 611183.

