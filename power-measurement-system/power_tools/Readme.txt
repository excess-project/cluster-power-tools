=======================================================
╔═╗╦  ╦ ╦╔═╗╔╦╗╔═╗╦═╗  ╔═╗╔═╗╦ ╦╔═╗╦═╗  ╔╦╗╔═╗╔═╗╦  ╔═╗
║  ║  ║ ║╚═╗ ║ ║╣ ╠╦╝  ╠═╝║ ║║║║║╣ ╠╦╝   ║ ║ ║║ ║║  ╚═╗
╚═╝╩═╝╚═╝╚═╝ ╩ ╚═╝╩╚═  ╩  ╚═╝╚╩╝╚═╝╩╚═   ╩ ╚═╝╚═╝╩═╝╚═╝
=======================================================

██████╗  ██████╗ ██╗    ██╗███████╗██████╗     
██╔══██╗██╔═══██╗██║    ██║██╔════╝██╔══██╗    
██████╔╝██║   ██║██║ █╗ ██║█████╗  ██████╔╝    
██╔═══╝ ██║   ██║██║███╗██║██╔══╝  ██╔══██╗    
██║     ╚██████╔╝╚███╔███╔╝███████╗██║  ██║    
╚═╝      ╚═════╝  ╚══╝╚══╝ ╚══════╝╚═╝  ╚═╝    
████████╗ ██████╗  ██████╗ ██╗     ███████╗    
╚══██╔══╝██╔═══██╗██╔═══██╗██║     ██╔════╝    
   ██║   ██║   ██║██║   ██║██║     ███████╗    
   ██║   ██║   ██║██║   ██║██║     ╚════██║    
   ██║   ╚██████╔╝╚██████╔╝███████╗███████║    
   ╚═╝    ╚═════╝  ╚═════╝ ╚══════╝╚══════╝    
                                               
High Performance Computing Center Stuttgart (HLRS)
           University of Stuttgart
		     &
                ADDI-DATA GmbH
             77836 Rheinmuenster

=======================================================
 This is a free Software.  Code is distributed in the
 hope that it will be useful,  but WITHOUT ANY WARRANTY;
 without even the implied warranty  of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. 
=======================================================

=======================================================
-How to analyse the output of the hpc_converter process
 (see ../driver/Readme.txt about hpc_converter) 
=======================================================
This package contains the tools to calculate the power 
consumption during a certain periode, to filter the
measured values, to concatenate the measured values,
which are stored in the sequencies of the files. 
=======================================================

================Compiling=============================
Choose a fortran compiler in Makefile. For example,
use gfortan of the GNU version 4.9.2 or later.
Set the path for the libhpcpower.a. We use this library 
to read the config #file, which also used by hpc_converter
(See ../driver/Readme.txt about hpc_converter).
The packages curl and ini_config must be also installes.
Call make.

===============Installing=============================
If you user Modules -- Software Environment Management
use the the module definition in the subdirectory ./opt
and copy into the proper directory.
copy the files to the directory, which is defined in 
module (under ./opt/..):
power_average - applies the median or mean filter on 
     measured values;
power_calculate - This tool helps to calculate a power consumption of various
     components for a certain period of time
power_calculate_avx -This tool helps to calculate a power consumption of
     various components for a certain period of time with the extention
     for the benchmark AVX (see (http://gitlab.excess-project.eu/benchmarks/avx-performance-benchmark)

==============How to use==============================
If you use the command "module load power/power_tools" loads the 
last version of the tools: module load power/power-tools

===================Power Average======================
-Power_average applies the median or mean filter for the power data.
The tool calculates for the values p from the files channel_X_*.dat the new
values:
S(i)=1/3(p(i-1) + p(i) + p(i+1); $window_step_left < i < $window_step_right
and saves it in the new file Channel_filter_median_X.dat. The input parameters
include:
-config_file: The full path to the file with the description of the components
-topdir: The directory contains the power raw data
-out_topdir: The output directory for the filtered data
-channel_id: The channel id, on which the filter should be applied
-out_prefixchannel: The prefix of the filtered data (default
     Channel_filter_median)
-board_id: The board id to which the channel belongs (see section Power
     measurement)
-operation: Name of the filter (median or mean)
-window_step_left: see filter definition above
-window_step_right: see filter definition above
-verbosity: 1 or 0 for silent mode

Example 1: Apply the mean filter on the power raw data for the voltage of the
first processor of node01 (channel 0; see attachment 1)
user@fe~:power_average -prefixchannel Channel -extension dat -channel_id 0\
-board_id 0 -topdir ./3223.fe.excess-project.eu/ -out_topdir\
./3223.fe.excess-project.eu -window_step_left 5 - window_step_right 5\
-operation mean -out_prefixchannel node01 _channel -verbosity 0

Example 2: Apply the median filter on the power raw data for the curent of the
first processor of node01 (channel 1; see attachment 1)
user@fe~:power_average -prefixchannel Channel -extension dat -channel_id 1\
-board_id 0 -topdir ./3223.fe.excess-project.eu/ -out_topdir\
./3223.fe.excess-project.eu -window_step_left 1 - window_step_right 1\
-operation median -out_prefixchannel node01 _channel -verbosity 0 

===================Power calculate=====================
This tool helps to calculate a power consumption of various components for
a certain period of time.
The periods or phases of the application can be defined in the arguments of
the tool or in the file with the timestamps of the certain periods.
Each period can be define with the start and end time since the 00:00:00
Coordinated Universal Time (UTC), Thursday, 1 January 1970 (POSIX time):
$offset_sec + $phase_start_sec +
$phase_start_microsec*1.0e-6+$phase_end_nanosec*1.0e-9 and
$offset_sec + $phase_end_sec +
$phase_end_microsec*1.0e-6+$phase_end_nanosec*1.0e-9 seconds
If the tool was configured with -relative_time 1 the time starts with the
beginning of the job executions.
You have also a possibility to define several phases of interest within a text
file.
The character ';' (ascii code: 59) must be used as delimiter.
user@fe:~/$cat profile.dat
id;start sec;start nanosec;end sec;end nanosec;
0; 1432141468; 531662094; 1432141468; 671346901;
1; 1432141468; 682023852; 1432141468; 804448747;

The arguments of the tool include:
-config_file: The full path to the file with the description of the components
  (see ../driver/Readme.txt about the format of the configure file)
-topdir: The directory contains the power raw data.
-profile_topdir: The directory contains the file with the timestamps of the
application phases.
-profile_file: The file with the timestamps of the application phases.
-offset_sec offset: see period definition above
-phase_start_sec: see period definition above
-phase_start_microsec: see period definition above
-phase_start_nanosec: see period definition above
-phase_end_sec: see period definition above
-phase_end_microsec: see period definition above
-phase_end_nanosec: see period definition above
-relative_time: 1 - insteed of the POSIX time use the time since start of the
executions job / power measurement 
If the profile file is used, than thw timestamps with a definition of the phase is
ignored.

Example:
Calculate the power consumption during the first second of the power
measurement:
user@fe~:power_calculate -config_file
"/opt/power/hpcmeasure/31/profiles/config_components_node01.ini"

    -topdir "./3223.fe.excess-project.eu/" -verbosity 0 -offset_sec 0.0
    -phase_start_sec 0.0 -phase_start_microsec 0.0 -phase_start_nanosec 0.0
    -phase_end_sec 1.0 -phase_end_microsec 0.0 -phase_end_nanosec 0.0
-relative_time 1

output:
=====phase:1=====
relative_time:T
phase_start: 0.000000E+00 sec
phase_end: 0.100000E+01 sec
phase_duration: 0.100000E+01 sec
phase_num_measures: 0.125000E+05

    ==component:CPU1_(W)_node01==
    average power: 0.205780E+02 watt
    power_by_average: 0.205807E+02 watt
    energy: 0.205780E+02 J
    energy_by_average: 0.205807E+02 J

    ==component:CPU2_(W)_node01==
    average power: 0.194467E+02 watt
    power_by_average: 0.194479E+02 watt
    energy: 0.194467E+02 J
    energy_by_average: 0.194479E+02 J

======================================================
This sub-directory contains:
- Readme.txt: You are reading it;
- Makefile
- soources fortran code of the tools
- opt/modulefiles/power/power_tools/25 
   Module file for the package Modules -- Software Environment Management

=======================================================
Bugs to: khabi@hlrs.de 

High Performance Computing Center Stuttgart (HLRS)
University of Stuttgart

======================================================
The used fortran module for the exploring of the command
line are programed by Uwe Küster (HLRS).
========================================================
Acknowledgment

This project is partially realized through EXCESS. 
EXCESS is funded by the EU 7th Framework Programme
(FP7/2013-2016) under grant agreement number 611183.
========================================================
