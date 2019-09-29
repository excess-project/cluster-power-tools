#!/bin/sh
module purge
module load compiler/intel/parallel_studio_2017_update_4
#module load compiler/intel/parallel_studio_xe_2018_update1_cluster_edition
set -x # Debug Modus ON
icc -O3 -g  -restrict -w -vec-report  -march=core-avx2  -qopenmp main.c io_profile.c dot_product_omp_atomic.c timer_profile.c cpufreq_user.c affinity.c  -DCPU_E5_2680V3  -lrt -lcpufreq -o main -qopt-report
icc -O3 -g  -restrict -w   -march=core-avx2 -g -W1  -S -fsource-asm -g  dot_product_omp_atomic.c -o dot_product_omp_atomic.c.S  -DCPU_E5_2680V3  
set +x # Debug Modus OFF

